#include <stdio.h>

#include <kernel/tty.h>

#include <stdarg.h>
#include <string.h>

static char *convert(unsigned int, int);

int printf(const char *format, ...) {
  va_list arg;
  va_start(arg, format);
  int written = 0;

  for (const char *traverse = format; *traverse != '\0'; traverse++) {
    if (*traverse != '%') {
      putchar(*traverse);
      written++;
      continue;
    }

    traverse++;

    int base;
    unsigned num;

    switch (*traverse) {
    case 'c': {
      unsigned i = va_arg(arg, int);
      putchar(i);
      written++;
      break;
    }

    case 's': {
      char *s = va_arg(arg, char *);
      term_print(s);
      break;
    }

    case 'd': {
      int i = va_arg(arg, int);

      if (i < 0) {
        i = -i;
        putchar('-');
        written++;
      }

      const char *converted = convert(i, 10);
      term_print(converted);
      written += strlen(converted);

      break;
    }

    case 'u':
      base = 10;
      goto print_unsigned_num;

    case 'x':
      base = 16;
      goto print_unsigned_num;

    case 'o':
      base = 8;
      goto print_unsigned_num;

    print_unsigned_num:
      num = va_arg(arg, unsigned);
      term_print(convert(num, base));
      written += strlen(convert(num, base));
      break;
    }
  }

  va_end(arg);

  return written;
}

static char *convert(unsigned int num, int base) {
  static const char Representation[] = "0123456789ABCDEF";
  static char buffer[50];

  char *ptr = buffer + 49;
  *ptr = '\0';

  do {
    if (ptr <= buffer) {
      break;
    }

    *--ptr = Representation[num % base];
    num /= base;
  } while (num != 0);

  return ptr;
}
