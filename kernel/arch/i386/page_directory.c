#include <kernel/panic.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define ENTRIES 1024

typedef uint32_t PDE;

typedef enum PDE_Flags {
    PDE_PRESENT         = 1<<0,
    PDE_WRITE_ENABLED   = 1<<1,
    PDE_SUPERVISOR_ONLY = 1<<2,
    PDE_PAGE_SIZE       = 1<<7,
} PDE_Flags;

extern struct __undefined __page_tables_physical;
extern PDE __page_dir[ENTRIES];

PDE* pde_get_by_linear_address(uintptr_t addr) {
    return __page_dir + (addr >> 22);
}

void pde_write_page_address(PDE *entry, uint32_t addr) {
    uint32_t aligned_addr = (addr >> 22) << 22;

    if (addr != aligned_addr) {
        panic("[PDE] Expected 4MB aligned address");
    }

    // Clear old base address and preserve flags
    PDE entry_val = *entry & 0xFFF;
    entry_val |= aligned_addr;
    entry_val |= PDE_PAGE_SIZE;

    *entry = entry_val;
}
