#include <stdint.h>

volatile uint16_t *vga_buffer = (uint16_t*)0xC00B8000;
const int VGA_COLS = 80;
const int VGA_ROWS = 25;
const int VGA_SIZE = VGA_COLS*VGA_ROWS;

int term_col;
int term_row;
uint8_t term_color = 0x1F;

void term_clear(void) {
    term_col = 0;
    term_row = 0;

    for (int i = 0; i<VGA_SIZE; i++) {
        vga_buffer[i] = ((uint16_t)term_color << 8) | ' ';
    }
}

void term_putc(char c) {
    if (term_col >= VGA_COLS) {
        term_row++, term_col = 0;
    }

    if (term_row >= VGA_ROWS) {
        term_row = term_col = 0;
    }

    if (c == '\n') {
        term_col = VGA_COLS; // endline
        return;
    }

    vga_buffer[term_row*VGA_COLS + (term_col++)] = ((int16_t)term_color << 8) | c;
}

void term_print(const char *str) {
    for (; *str; str++) {
        term_putc(*str);
    }
}
