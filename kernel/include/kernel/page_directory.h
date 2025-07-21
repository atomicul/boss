#ifndef _KERNEL_PAGE_DIRECTORY_H
#define _KERNEL_PAGE_DIRECTORY_H

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

void pde_init(void);
PDE* pde_get_by_linear_address(uintptr_t addr);
void pde_write_page_address(PDE *entry, uint32_t addr);
void pde_write_table_address(PDE *entry, uint32_t addr);

#endif
