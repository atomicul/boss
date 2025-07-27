#include <string.h>

#include <stddef.h>

size_t strlen(const char* str) {
    size_t out = 0;
    for(; *str; str++)
        out++;
    return out;
}
