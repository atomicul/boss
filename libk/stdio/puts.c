#include <stdio.h>

int puts(const char* str) {
    int written = 0;

    for (; *str; str++) {
        putchar(*str);
        written++;
    }

    putchar('\n');
    written++;

    return written;
}
