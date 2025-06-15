#include <stdio.h>
#include <kernel/tty.h>

int putchar(int ic) {
	char c = (char) ic;
	term_putc(c);
	return ic;
}
