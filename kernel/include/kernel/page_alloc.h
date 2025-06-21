#ifndef _KERNEL_PAGE_FRAME_ALLOC_H
#define _KERNEL_PAGE_FRAME_ALLOC_H

#include <stdint.h>

void init_frame_alloc(void);
uintptr_t alloc_frame(void);
void free_frame(uintptr_t frame_addr);

#endif
