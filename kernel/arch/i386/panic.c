#include <kernel/panic.h>
#include <kernel/tty.h>

#include <stdio.h>
#include <stdbool.h>

#define NULL 0

void panic(const char* message) {
    if (message == NULL) {
        term_print("Kernel panic! :(");
    } else {
        term_print("Kernel panic!: ");
        puts(message);
    }

    while (true) {
        asm volatile ("hlt");
    }
}
