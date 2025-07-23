#include <kernel/panic.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#define ENTRIES 1024

typedef uint32_t PDE;

typedef enum PDE_Flags {
    PDE_PRESENT         = 1<<0,
    PDE_WRITE_ENABLED   = 1<<1,
    PDE_SUPERVISOR_ONLY = 1<<2,
    PDE_PAGE_SIZE       = 1<<7,
} PDE_Flags;

extern struct __undefined endkernel;
extern struct __undefined page_tables;
extern PDE page_dir[ENTRIES];

PDE* pde_get_by_linear_address(uintptr_t addr) {
    return page_dir + (addr >> 22);
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

void pde_write_table_address(PDE *entry) {
    ptrdiff_t entry_index = entry - page_dir;
    const size_t table_entry_size = 4;
    const size_t table_entries = 1024;
    const uintptr_t tables_addr = (uintptr_t)&page_tables;

    const uintptr_t table_addr = tables_addr + table_entry_size*table_entries*entry_index;

    // Clear old base address and preserve flags
    PDE entry_val = *entry & 0xFFF;
    entry_val |= table_addr;
    entry_val &= ~(PDE)PDE_PAGE_SIZE;

    *entry = entry_val;
}

void pde_init(void) {
    // Identity map first 4MB
    pde_write_page_address(page_dir, 0);
    page_dir[0] |= PDE_PRESENT | PDE_WRITE_ENABLED | PDE_SUPERVISOR_ONLY;

    // Clear the rest of the page directory
    memset(page_dir+1, 0, ENTRIES - 1);

    // Map the kernel in the higher half
    const uintptr_t kernel_position = 0xC0000000;

    PDE* entry = pde_get_by_linear_address(kernel_position);

    uintptr_t physical_addr = 0;
    uintptr_t endkernel_addr = (uintptr_t)&endkernel;
    for (; endkernel_addr > physical_addr; entry++, physical_addr += 1 << 22) {
        pde_write_page_address(entry, physical_addr);
        *entry |= PDE_PRESENT | PDE_WRITE_ENABLED | PDE_SUPERVISOR_ONLY;
    }
}
