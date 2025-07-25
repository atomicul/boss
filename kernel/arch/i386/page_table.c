#include <kernel/panic.h>
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
    PTE_SUPERVISOR_ONLY = 1<<2,
} PTE_Flags;

extern PTE __page_tables[TABLES*ENTRIES];

PTE* pte_get_by_linear_address(uintptr_t addr) {
    const size_t entry_idx = addr >> 12;

    return __page_tables  + entry_idx;
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
