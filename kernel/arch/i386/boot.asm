extern kernel_main
extern enable_paging
extern pma_init
extern term_clear
extern __page_dir

MBALIGN  equ  1 << 0
MEMINFO  equ  1 << 1
MBFLAGS  equ  MBALIGN | MEMINFO
MAGIC    equ  0x1BADB002
CHECKSUM equ -(MAGIC + MBFLAGS)

section .multiboot
align 4
    dd MAGIC
    dd MBFLAGS
    dd CHECKSUM

section .bss
align 16
stack_bottom:
resb 16384 ; 16 KiB is reserved for stack
stack_top:

section .boot
global _start
_start:
    mov esp, stack_top - 0xC0000000

    call enable_paging

    jmp after_paging

section .text
after_paging:
    mov [__page_dir], dword 0 ; clear temporary identity paging

    mov esp, stack_top

    call term_clear

    call pma_init

    call kernel_main

    cli
.hang:
    hlt
    jmp .hang
