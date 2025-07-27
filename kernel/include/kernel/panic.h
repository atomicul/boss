#ifndef _KERNEL_PANIC_H
#define _KERNEL_PANIC_H

#include <stdnoreturn.h>

noreturn void panic(const char *message);

#endif
