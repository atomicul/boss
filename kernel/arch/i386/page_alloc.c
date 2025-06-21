#include <kernel/page_alloc.h>
#include <kernel/panic.h>

#include <stdint.h>

#define SIZE 128

#define KTH_BIT(arr, k) (arr[k/32]>>(k%32) & 1)
#define TOGGLE_KTH_BIT(arr, k) (arr[k/32] ^= 1<<(k%32))

extern const uintptr_t endkernel;
uintptr_t frames_start;

uint32_t page_bitmap[SIZE];

static inline uintptr_t next_aligned_frame(uintptr_t frame) {
    if (frame & 0xFFF) {
        frame &= ~(uintptr_t)0xFFF;
        frame += 4096;
    }

    return frame;
}

void init_frame_alloc(void) {
    if (frames_start)
        return;

    frames_start = next_aligned_frame(endkernel);
}

uintptr_t alloc_frame(void) {
    for (int i = 0; i < SIZE; i++) {
        if (page_bitmap[i] == ~(uint32_t)0) {
            // all 32 pages already allocated
            continue;
        }

        for (int j = 0; j<32; j++) {
            const int bit = i*32+j;

            if (KTH_BIT(page_bitmap, bit) == 0) {
                TOGGLE_KTH_BIT(page_bitmap, bit);
                return frames_start+bit*4096;
            }
        }
    }

    panic("Ran out of pages to allocate");
}

void free_frame(uintptr_t frame_addr) {
    if (frame_addr & 0xFFF) {
        panic("Received call to free misalligned frame");
    }

    if (frame_addr < frames_start) {
        panic("Received call to free out of bounds frame");
    }

    const int bit = (frame_addr-frames_start)/4096;

    if (bit / 32 >= SIZE) {
        panic("Received call to free out of bounds frame");
    }

    if (KTH_BIT(page_bitmap, bit) == 0) {
        panic("Attempted to free already free frame");
    }

    TOGGLE_KTH_BIT(page_bitmap, bit);
}
