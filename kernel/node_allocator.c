#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define MAX_SIZE(t) (((0x1ULL << ((sizeof(t) * 8ULL) - 1ULL)) - 1ULL) | \
                    (0xFULL << ((sizeof(t) * 8ULL) - 4ULL)))

#define LIST_TERMINATOR MAX_SIZE(size_t)

typedef struct Header {
    size_t buff_size;
    size_t block_body_size;
    uintptr_t free_blocks_head;
} Header;

typedef struct BlockHeader {
    size_t next_free_block;
} BlockHeader;

static inline size_t block_size(void *buff) {
    Header *header = (Header*)buff;
    return header->block_body_size + sizeof(BlockHeader);
}

static inline Header* header(void *buff) {
    return (Header*)buff;
}

static inline uintptr_t body(void *buff) {
    return (uintptr_t)buff + sizeof(Header);
}

static inline BlockHeader* block_header(uintptr_t block) {
    return (BlockHeader*)block;
}

static inline uintptr_t block_from_body(void *block_body) {
    return (uintptr_t)block_body - sizeof(BlockHeader);
}

static inline void* block_body(uintptr_t block) {
    return (void*)(block + sizeof(BlockHeader));
}

static inline bool is_valid_block(void *buff, uintptr_t block) {
    if (body(buff) > block) {
        return false;
    }

    uintptr_t buff_end = (uintptr_t)buff + header(buff)->buff_size;
    uintptr_t block_end = block + block_size(buff);

    if (block_end > buff_end) {
        return false;
    }

    bool is_aligned = (block-body(buff)) % block_size(buff) == 0;
    if (!is_aligned) {
        return false;
    }

    return true;
}

void nalloc_init(void *buff, size_t buff_size, size_t node_size) {
    Header *h = header(buff);

    h->buff_size = buff_size;
    h->free_blocks_head = body(buff);
    h->block_body_size = node_size;

    for (uintptr_t block = body(buff); is_valid_block(buff, block); block += block_size(buff)) {
        uintptr_t next_block = block + block_size(buff);

        BlockHeader* block_header = (BlockHeader*)block;
        block_header->next_free_block = is_valid_block(buff, next_block) ? next_block : LIST_TERMINATOR;
    }
}

void* nalloc_get_node(void *buff) {
    if (header(buff)->free_blocks_head == LIST_TERMINATOR) {
        return NULL;
    }

    uintptr_t free_block = header(buff)->free_blocks_head;

    Header *h = header(buff);
    h->free_blocks_head = block_header(free_block)->next_free_block;

    return block_body(free_block);
}

void nalloc_free_node(void *buff, void *node) {
    Header *h = header(buff);
    uintptr_t block = block_from_body(node);
    BlockHeader *bh = block_header(block);

    bh->next_free_block = h->free_blocks_head;
    h->free_blocks_head =  block;
}
