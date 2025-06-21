#include <stddef.h>
#include <stdint.h>

#include <kernel/tty.h>
#include <kernel/page_alloc.h>

#include <stdio.h>

#if defined(__linux__)
#error "This code must be compiled with a cross-compiler"
#elif !defined(__i386__)
#error "This code must be compiled with an x86-elf compiler"
#endif

void kernel_main(void) {
    init_frame_alloc();
    term_clear(); // init terminal

    free_frame(0);
    puts("Hello, LinkedIn!");
    puts("Welcome to my kernel.");
}
