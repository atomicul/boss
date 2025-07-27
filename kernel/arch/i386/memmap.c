#include <kernel/page_directory.h>
#include <kernel/page_table.h>
#include <kernel/panic.h>
#include <kernel/physical_memory_allocator.h>
#include <kernel/node_allocator.h>

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#define KERNEL_START 0xC0000000
#undef NULL
#define NULL 0

uint8_t nodes_buff[2048];

typedef struct FreeSpace {
    uintptr_t address;
    size_t size;
    struct FreeSpace *next;
} FreeSpace;

FreeSpace *free_list;


typedef struct AllocatedSpace {
    size_t size;
} AllocatedSpace;

AllocatedSpace alloc_table[KERNEL_START/FRAME_SIZE];

static inline void compute_page_alloc(size_t *bytes, size_t *page_count, size_t *huge_page_count) {
    *huge_page_count = *bytes / HUGE_FRAME_SIZE;
    *bytes -= *huge_page_count * HUGE_FRAME_SIZE;

    *page_count = *bytes / FRAME_SIZE;
    *bytes -= *page_count * FRAME_SIZE;

    if (*bytes > 0) {
        (*page_count)++;
    }

    *bytes = *huge_page_count*HUGE_FRAME_SIZE + *page_count*FRAME_SIZE;
}

static inline uintptr_t get_free_space(size_t bytes, size_t alignment) {
    for (FreeSpace *i = free_list, *prev = NULL;
            i != NULL;
            prev=i, i=prev->next) {

        uintptr_t begin; // Potential return address
        if (i->address % alignment == 0) {
            begin = i->address;
        } else {
            begin = i->address - (i->address % alignment) + alignment;
        }

        if (i->address + i->size - begin < bytes) {
            // Not a fit, continue
            continue;
        }

        if (begin > i->address) {
            // Split the node to continue working on an aligned one

            if (prev == NULL) {
                prev = nalloc_get_node(nodes_buff);
                free_list = prev;
            } else {
                prev->next = nalloc_get_node(nodes_buff);
                prev = prev->next;
            }

            prev->next = i;
            prev->address = i->address;
            prev->size = begin - prev->address;

            i->address = begin;
            i->size -= prev->size;
        }

        if (i->size == bytes) {
            // Perfect size, remove the entire node

            if (prev == NULL) {
                free_list = i->next;
            } else {
                prev->next = i->next;
            }

            uintptr_t out = i->address;
            nalloc_free_node(nodes_buff, i);       
            return out;
        }

        // Larger size, resize the node
        uintptr_t out = i->address;
        i->address += bytes;
        i->size -= bytes;
        return out;
    }

    return NULL;
}

static inline void reclaim_free_space(uintptr_t addr, size_t size) {
    FreeSpace *i = free_list, *prev = NULL;
    for (; i != NULL || prev != NULL;
         prev=i, i=prev == NULL ? NULL : prev->next) {

        bool aligned_behind = prev == NULL || prev->address + prev->size <= addr;
        bool aligned_ahead = i == NULL || addr + size <= i->address;

        if (aligned_behind && aligned_ahead) {
            break;
        }
    }

    // Insert node here
    FreeSpace *new_node = nalloc_get_node(nodes_buff);

    if (prev == NULL) {
        free_list = new_node;
    } else {
        prev->next = new_node;
    }

    new_node->address = addr;
    new_node->size = size;
    new_node->next = i;

    i = new_node;

    // Try merge with the previous node
    if (prev != NULL && prev->address + prev->size >= i->address) {
        prev->next = i->next;
        prev->size = i->address + i->size - prev->address;

        nalloc_free_node(nodes_buff, i);
        i = prev;
        prev = NULL;
    }

    // Try merge with the next node
    if (i->next != NULL && i->address + i->size >= i->next->address) {
        FreeSpace *p = i->next;
        i->next = p->next;
        i->size = p->address + p->size - i->address;

        nalloc_free_node(nodes_buff, p);
    }
}

void *memmap(size_t bytes) {
    size_t page_count, huge_page_count;
    compute_page_alloc(&bytes, &page_count, &huge_page_count);

    const uintptr_t address = get_free_space(bytes, huge_page_count > 0 ? HUGE_FRAME_SIZE : FRAME_SIZE);

    if (address == NULL) {
        return NULL;       
    }
    
    uintptr_t normal_pages_start = address + HUGE_FRAME_SIZE*huge_page_count;

    for (PDE *dir_entry = pde_get_by_linear_address(address);
            huge_page_count--;
            dir_entry++) {

        // TODO: Handle out of physical memory errors
        pde_write_page_address(dir_entry, pma_get_frame(PMA_HUGE));

        *dir_entry |= PDE_WRITE_ENABLED | PDE_PRESENT;
    }

    for (PTE *table_entry = pte_get_by_linear_address(normal_pages_start);
            page_count--;
            table_entry++) {

        // TODO: Handle out of physical memory errors
        pte_write_page_address(table_entry, pma_get_frame(PMA_NORMAL));

        *table_entry |= PDE_WRITE_ENABLED | PDE_PRESENT;
    }

    alloc_table[address/FRAME_SIZE].size = bytes;
    return (void*)address;
}

void memfree(void *address) {
    AllocatedSpace *entry = alloc_table + (uintptr_t)address/FRAME_SIZE;

    if (entry->size == 0) {
        panic("[memfree] Attempted to free already free memory");
    }

    size_t pages, huge_pages;
    compute_page_alloc(&entry->size, &pages, &huge_pages);

    uintptr_t normal_pages_start = (uintptr_t)address + HUGE_FRAME_SIZE*huge_pages;

    for (PDE *dir_entry = pde_get_by_linear_address((uintptr_t)address);
            huge_pages--;
            dir_entry++) {

        pma_free_frame(*dir_entry >> 22 << 22);

        *dir_entry = 0;
    }

    for (PTE *table_entry = pte_get_by_linear_address(normal_pages_start);
            pages--;
            table_entry++) {

        pma_free_frame(*table_entry >> 12 << 12);

        *table_entry = 0;
    }

    reclaim_free_space((uintptr_t)address, entry->size);
    entry->size = 0;
}

uintptr_t memphys(void *addr) {
    uintptr_t addr_int = (uintptr_t)addr;

    PDE *pde = pde_get_by_linear_address(addr_int);

    if (!(*pde & PDE_PRESENT)) {
        return NULL;
    }

    if (*pde & PDE_PAGE_SIZE) {
        uintptr_t offset = ((1<<22) - 1)&addr_int;
        uintptr_t frame_addr = *pde >> 22 << 22;

        return frame_addr + offset;
    }

    PTE *pte = pte_get_by_linear_address(addr_int);

    if (!(*pte & PTE_PRESENT)) {
        return NULL;
    }

    uintptr_t offset = ((1<<12) - 1)&addr_int;
    uintptr_t frame_addr = *pte >> 12 << 12;

    return frame_addr + offset;
}

void mem_init(void) {
    nalloc_init(nodes_buff, sizeof(nodes_buff), sizeof(FreeSpace));

    free_list = (FreeSpace*)nalloc_get_node(nodes_buff);

    free_list->next = NULL;
    free_list->address = HUGE_FRAME_SIZE;
    free_list->size = KERNEL_START  - free_list->address;
}

void print_debug(void) {
    for (FreeSpace *i = free_list; i != NULL; i = i->next) {
        printf("FreeSpace 0x%x: { addr=0x%x, size=%u }\n", i, i->address, i->size);
    }
}
