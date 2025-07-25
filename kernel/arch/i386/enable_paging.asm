section .boot
global enable_paging
extern __page_dir_physical
extern __endkernel_physical

ENTRY_SIZE       equ 4

KERNEL_ADDR      equ 0xC0000000

; Page directory/table entry flags
PDE_PRESENT      equ 1 << 0   ; Present bit
PDE_WRITABLE     equ 1 << 1   ; Read/Write bit
PDE_SUPERVISOR   equ 1 << 2   ; Kernel access only
PDE_PAGESIZE     equ 1 << 7   ; 4MB page

; Control register flags
CR0_PG           equ 1 << 31  ; Paging enable bit
CR0_WP           equ 1 << 16  ; Protect read only pages writes from supervisor
CR4_PSE          equ 1 << 4   ; Enables 4MB pages

enable_paging:
    ; Identity map first directory entry
    mov [__page_dir_physical], dword (PDE_PRESENT | PDE_WRITABLE | PDE_PAGESIZE | PDE_SUPERVISOR)

    ; Map kernel in higher half
    mov edi, __page_dir_physical + ENTRY_SIZE*(KERNEL_ADDR >> 22)
    xor eax, eax
    mov edx, PDE_PRESENT | PDE_WRITABLE | PDE_PAGESIZE | PDE_SUPERVISOR

.set_entry:
    mov [edi], eax                ; Store lower 32 bits of entry (address bits 31-22)
    or [edi], edx                 ; Combine address with flags
    add eax, 0x400000             ; Next 4MB physical address
    add edi, ENTRY_SIZE           ; Move to next directory entry

    cmp eax, __endkernel_physical
    jle .set_entry

    mov eax, __page_dir_physical
    mov cr3, eax

    mov eax, cr4
    or eax, CR4_PSE
    mov cr4, eax

    mov eax, cr0
    or eax, CR0_WP | CR0_PG
    mov cr0, eax
    
    ret
