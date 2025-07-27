#include <string.h>

#include <stddef.h>

void *memcpy(void *dest, const void *src, size_t count) {
    while(count--) {
        *(char *)dest++ = *(char *)src++;
    }
    return dest;
}
