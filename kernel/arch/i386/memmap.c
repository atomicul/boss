#include <kernel/page_directory.h>
#include <kernel/page_table.h>
#include <kernel/physical_memory_allocator.h>
#include <kernel/node_allocator.h>

#include <stddef.h>

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

void *memmap(size_t bytes) {
    size_t huge_page_count = bytes / HUGE_FRAME_SIZE;
    bytes -= huge_page_count * HUGE_FRAME_SIZE;

    size_t page_count = bytes / FRAME_SIZE;
    bytes -= page_count * FRAME_SIZE;

    if (bytes > 0) {
        page_count++;
    }

    bytes = huge_page_count*HUGE_FRAME_SIZE + page_count*FRAME_SIZE;

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

    return (void*)address;
}

void mem_init(void) {
    nalloc_init(nodes_buff, sizeof(nodes_buff), sizeof(FreeSpace));

    free_list = (FreeSpace*)nalloc_get_node(nodes_buff);

    free_list->next = NULL;
    free_list->address = HUGE_FRAME_SIZE;
    free_list->size = KERNEL_START  - free_list->address;
}
