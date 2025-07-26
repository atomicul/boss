#ifndef _KERNEL_MEMMAP_H
#define _KERNEL_MEMMAP_H

#include <stddef.h>

void *memmap(size_t bytes);

void memfree(void *memory);

#endif
