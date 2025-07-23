#ifndef _KERNEL_PAGE_FRAME_ALLOC_H
#define _KERNEL_PAGE_FRAME_ALLOC_H

#include <stdint.h>

#define FRAME_SIZE 0x1000
#define HUGE_FRAME_SIZE 0x400000

typedef enum PMA_FrameSize {
    PMA_NORMAL = 0,
    PMA_HUGE = 1,
} PMA_FrameSize;

void pma_init(void);
uintptr_t pma_alloc_frame(PMA_FrameSize frame_size);
void pma_free_frame(uintptr_t frame_addr);

#endif
