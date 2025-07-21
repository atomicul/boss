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

extern PDE page_dir[ENTRIES];

PDE* pde_get_by_linear_address(uintptr_t addr) {
    return page_dir + (addr >> 22);
}

void pde_write_page_address(PDE *entry, uint32_t addr) {
    uint32_t aligned_addr = (addr >> 22) << 22;

    if (addr != aligned_addr) {
        panic("Expected 4MB aligned address");
    }

    // Clear old base address and preserve flags
    PDE entry_val = *entry & 0x003FFFFF;
    entry_val |= aligned_addr;
    entry_val |= PDE_PAGE_SIZE;

    *entry = entry_val;
}

void pde_write_table_address(PDE *entry, uint32_t addr) {
    uint32_t aligned_addr = (addr >> 12) << 12;

    if (addr != aligned_addr) {
        panic("Expected 4KB aligned address");
    }

    // Clear old base address and preserve flags
    PDE entry_val = *entry & 0x003FFFFF;
    entry_val |= aligned_addr;
    entry_val |= PDE_PAGE_SIZE;

    *entry = entry_val;
}

void pde_init(void) {
    const uintptr_t kernel_position = 0xC0000000;

    PDE* entry = pde_get_by_linear_address(kernel_position);

    uintptr_t physical_addr = 0;
    for (int i = 0; i<16; i++, entry++) {
        pde_write_page_address(entry, physical_addr);
        *entry |= PDE_PRESENT | PDE_WRITE_ENABLED;
        physical_addr += 1 << 22;
    }
}
