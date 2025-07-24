#ifndef _KERNEL_NODE_ALLOCATOR_H
#define _KERNEL_NODE_ALLOCATOR_H

#include <stddef.h>

void nalloc_init(void *buff, size_t buff_size, size_t node_size);
void* nalloc_get_node(void *buff);
void nalloc_free_node(void *buff, void *node);

#endif
