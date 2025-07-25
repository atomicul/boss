#ifndef _KERNEL_PAGE_TABLE_H
#define _KERNEL_PAGE_TABLE_H

#include <stddef.h>
#include <stdint.h>

typedef uint32_t PTE;

typedef enum PTE_Flags {
    PTE_PRESENT         = 1<<0,
    PTE_WRITE_ENABLED   = 1<<1,
    PTE_SUPERVISOR_ONLY = 1<<2,
} PTE_Flags;

PTE* pte_get_by_linear_address(uintptr_t addr);
void pte_write_page_address(PTE *entry, uint32_t addr);

#endif
