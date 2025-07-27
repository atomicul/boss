#ifndef _KERNEL_MEMMAP_H
#define _KERNEL_MEMMAP_H

#include <stddef.h>
#include <stdint.h>

void *memmap(size_t bytes);

void memfree(void *memory);

uintptr_t memphys(void *linear_address);

#endif
