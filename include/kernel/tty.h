#ifndef _KERNEL_TTY_H
#define _KERNEL_TTY_H

void term_clear(void);

void term_putc(char c);

void term_print(const char *str);

#endif
