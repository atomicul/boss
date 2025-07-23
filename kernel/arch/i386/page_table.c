#include <kernel/panic.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define KERNEL_ADDRESS 0xC0000000
#define TABLES 1024
#define ENTRIES 1024
typedef uint32_t PTE;

typedef enum PTE_Flags {
    PTE_PRESENT         = 1<<0,
    PTE_WRITE_ENABLED   = 1<<1,
    PTE_SUPERVISOR_ONLY = 1<<2,
} PTE_Flags;

extern struct __undefined endkernel;
extern struct __undefined page_tables;

PTE* pte_get_by_linear_address(uintptr_t addr) {
    const uintptr_t page_tables_physical_addr = (uintptr_t)&page_tables;
    PTE * const page_tables_virtual_addr = (PTE*)(page_tables_physical_addr + KERNEL_ADDRESS);
    const size_t entry_idx = addr >> 12;

    return page_tables_virtual_addr  + entry_idx;
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
}

void pte_init(void) {
    memset(pte_get_by_linear_address(0), 0, TABLES*ENTRIES);
}
