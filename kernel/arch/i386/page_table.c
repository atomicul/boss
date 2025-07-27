#include <kernel/panic.h>
#include <kernel/page_directory.h>

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define TABLES 1024
#define ENTRIES 1024
typedef uint32_t PTE;

typedef enum PTE_Flags {
    PTE_PRESENT         = 1<<0,
    PTE_WRITE_ENABLED   = 1<<1,
    PTE_USER_ACCESS     = 1<<2,
} PTE_Flags;

extern PTE __page_tables[TABLES*ENTRIES];
extern struct __undefined __page_tables_physical;

PTE* pte_get_by_linear_address(uintptr_t addr) {
    const size_t entry_idx = addr >> 12;

    return __page_tables  + entry_idx;
}

static inline uintptr_t linear_address(const PTE *entry) {
    size_t entry_idx = entry - __page_tables;
    return entry_idx << 12;
}

static inline uintptr_t physical_address(const PTE *entry) {
    size_t entry_idx = entry - __page_tables;
    return (uintptr_t)&__page_tables_physical + sizeof(PTE)*entry_idx;
}

void pte_write_page_address(PTE *entry, uint32_t addr) {
    uint32_t aligned_addr = (addr >> 12) << 12;

    if (addr != aligned_addr) {
        panic("[PTE] Expected 4KB aligned address");
    }

    // Clear old base address and preserve flags
    PTE entry_val = *entry & 0xFFF;
    entry_val |= aligned_addr;

    *entry = entry_val;

    PDE *pde = pde_get_by_linear_address(linear_address(entry));
    PDE pde_cpy = *pde & 0xFFF;
    // Set most permissive settings to control access in the table entry
    pde_cpy |= PDE_PRESENT | PDE_WRITE_ENABLED | PDE_USER_ACCESS;
    pde_cpy &= ~(PDE)PDE_PAGE_SIZE;
    pde_cpy |= physical_address(entry);
    *pde = pde_cpy;
}
