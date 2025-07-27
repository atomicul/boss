#include <string.h>

#include <stddef.h>

void *memset(void *dest, int ch, size_t count) {
    while(count--) {
        *(char *)dest++ = (char)ch;
    }
    return dest;
}
