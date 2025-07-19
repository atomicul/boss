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

section .text
extern enable_paging
global _start:function (_start.end - _start)
_start:
	mov esp, stack_top

    call enable_paging

	extern kernel_main
	call kernel_main

	cli
.hang:
    hlt
	jmp .hang
.end:
