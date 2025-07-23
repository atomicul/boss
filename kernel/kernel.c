#include <stddef.h>
#include <stdint.h>

#include <kernel/tty.h>
#include <kernel/physical_memory_allocator.h>
#include <kernel/page_directory.h>
#include <kernel/page_table.h>

#include <stdio.h>

#if defined(__linux__)
#error "This code must be compiled with a cross-compiler"
#elif !defined(__i386__)
#error "This code must be compiled with an x86-elf compiler"
#endif

void kernel_main(void) {
    term_clear(); // init terminal
    pde_init();
    pte_init();
    pma_init();

    puts("Hello, LinkedIn!");
    puts("Welcome to my kernel.");
}
