#include <string.h>

#include <stddef.h>

char *strcat(char *dest, const char *src) {
    strcpy(dest+strlen(dest), src);
    return dest;
}
