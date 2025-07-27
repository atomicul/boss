ENTRY(_start) 

SECTIONS
{
    . = 1M;

    __kernel_start = .;
    .multiboot :
    {
        KEEP(*(.multiboot))
        *(.boot)
    }

    . += 0xC0000000;

    .text BLOCK(4K) : AT(ADDR(.text) - 0xC0000000)
    {
        *(.text)
    }

    .rodata BLOCK(4K) : AT(ADDR(.rodata) - 0xC0000000)
    {
        *(.rodata)
    }

    .data BLOCK(4K) : AT(ADDR(.data) - 0xC0000000)
    {
        *(.data)
    }

    .bss BLOCK(4K) : AT(ADDR(.bss) - 0xC0000000)
    {
        *(COMMON)
        *(.bss)

        . = ALIGN(4K);
        __page_dir = .;
        __page_dir_physical = . - 0xC0000000;
        . += 4K;

        . = ALIGN(4K);
        __page_tables = .;
        __page_tables_physical = . - 0xC0000000;
        . += 4M;
        __endkernel = .;
        __endkernel_physical = . - 0xC0000000;
    }
}
