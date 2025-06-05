#include <stddef.h>
#include <stdint.h>

#include <kernel/tty.h>

#if defined(__linux__)
#error "This code must be compiled with a cross-compiler"
#elif !defined(__i386__)
#error "This code must be compiled with an x86-elf compiler"
#endif

void kernel_main(void) {
    term_clear();

    term_print("Hello, LinkedIn!\n");
    term_print("Welcome to my kernel.\n");
}
