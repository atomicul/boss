#include <kernel/memmap.h>

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#if defined(__linux__)
#error "This code must be compiled with a cross-compiler"
#elif !defined(__i386__)
#error "This code must be compiled with an x86-elf compiler"
#endif

uint8_t byte;

void kernel_main(void) {
    printf("Random static kernel variable address: 0x%x\n", &byte);

    const char *message = "I'm in the higher half :D";
    char* memory = memmap(strlen(message)+1);

    strcpy(memory, message);

    puts(memory);

    printf("Message physical address: 0x%x\n", memphys(memory));
    printf("Message virtual address: 0x%x\n", memory);

    memfree(memory);
}
