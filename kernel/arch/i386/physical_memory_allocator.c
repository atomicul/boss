#include <kernel/panic.h>

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#define FRAME_SIZE 0x1000
#define HUGE_FRAME_SIZE 0x400000

#define FRAMES_BITSET_SIZE 8192
#define HUGE_FRAMES_BITSET_SIZE 8

#define KTH_BIT(arr, k) (arr[k/32]>>(k%32) & 1)
#define TOGGLE_KTH_BIT(arr, k) (arr[k/32] ^= 1<<(k%32))

typedef uint32_t BitsetChunk;
#define CHUNK_BITS (8*sizeof(BitsetChunk))

extern struct __undefined __endkernel_physical;
uintptr_t frames_start;
uintptr_t huge_frames_start;

BitsetChunk frame_bitset[FRAMES_BITSET_SIZE];
BitsetChunk huge_frame_bitset[HUGE_FRAMES_BITSET_SIZE];

typedef enum PMA_FrameSize {
    PMA_NORMAL = 0,
    PMA_HUGE = 1,
} PMA_FrameSize;

static inline uintptr_t next_aligned_huge_frame(uintptr_t frame) {
    const uintptr_t mask = (1 << 22) - 1;
    if (frame & mask) {
        frame &= ~mask;
        frame += (mask + 1);
    }
    return frame;
}

void pma_init(void) {
    if (frames_start)
        return;

    frames_start = next_aligned_huge_frame((uintptr_t)&__endkernel_physical);
    huge_frames_start = next_aligned_huge_frame(frames_start + 8*sizeof(frame_bitset)*FRAME_SIZE);
}

static inline size_t turn_on_first_available_bit(BitsetChunk *bitset, size_t size) {
    for (size_t i = 0; i < size; i++) {
        if (frame_bitset[i] == ~(BitsetChunk)0) {
            continue;
        }

        for (size_t j = 0; j<CHUNK_BITS; j++) {
            const int bit = i*CHUNK_BITS+j;

            if (KTH_BIT(bitset, bit) == 0) {
                TOGGLE_KTH_BIT(bitset, bit);
                return bit;
            }
        }
    }

    panic("[PMA] Ran out of frame to allocate");
}

uintptr_t pma_get_frame(PMA_FrameSize frame_size) {
    if (frame_size == PMA_NORMAL) {
        size_t frame_idx = turn_on_first_available_bit(frame_bitset, FRAMES_BITSET_SIZE);
        return frames_start + FRAME_SIZE*frame_idx;
    }

    if (frame_size == PMA_HUGE) {
        size_t frame_idx = turn_on_first_available_bit(huge_frame_bitset, HUGE_FRAMES_BITSET_SIZE);
        return huge_frames_start + HUGE_FRAME_SIZE*frame_idx;
    }

    panic("[PMA] Got unexpected enum argument");
}

static inline void mark_frame_free_in_bitset(PMA_FrameSize size, uintptr_t frame_addr) {
    BitsetChunk *bitset = size == PMA_NORMAL ? frame_bitset : huge_frame_bitset;
    size_t bitset_size = size == PMA_NORMAL ? FRAMES_BITSET_SIZE : HUGE_FRAMES_BITSET_SIZE;
    size_t frame_size = size == PMA_NORMAL ? FRAME_SIZE : HUGE_FRAME_SIZE;
    uintptr_t frames_start_addr = size == PMA_NORMAL ? frames_start : huge_frames_start;

    if (frame_addr & (frame_size - 1)) {
        panic("[PMA] Received call to free misalligned frame");
    }

    const int bit = (frame_addr-frames_start_addr)/frame_size;

    if (bit / CHUNK_BITS >= bitset_size) {
        panic("[PMA] Received call to free out of bounds frame");
    }

    if (KTH_BIT(bitset, bit) == 0) {
        panic("[PMA] Attempted to free already free frame");
    }

    TOGGLE_KTH_BIT(bitset, bit);
}

void pma_free_frame(uintptr_t frame_addr) {
    if (frame_addr < frames_start) {
        panic("[PMA] Received call to free out of bounds frame");
    }

    bool is_huge_frame = frame_addr >= huge_frames_start;

    if (is_huge_frame) {
        mark_frame_free_in_bitset(PMA_HUGE, frame_addr);

        return;
    }

    mark_frame_free_in_bitset(PMA_NORMAL, frame_addr);
}
