section .text
global enable_paging
extern __page_dir

; Page directory/table entry flags
PDE_PRESENT    equ 1 << 0   ; Present bit
PDE_WRITABLE   equ 1 << 1   ; Read/Write bit
PDE_PAGESIZE   equ 1 << 7   ; 4MB page

; Control register flags
CR0_PG         equ 1 << 31  ; Paging enable bit
CR0_WP         equ 1 << 16  ; Protect read only pages writes from supervisor
CR4_PSE        equ 1 << 4   ; Enables 4MB pages

enable_paging:
    ; Set up page directory
    mov edi, __page_dir             ; Load page directory start address
    mov ecx, 1024                 ; 1024 entries in page directory
    xor eax, eax                  ; Start with physical address 0
    mov edx, PDE_PRESENT | PDE_WRITABLE | PDE_PAGESIZE

.set_entry:
    mov [edi], eax                ; Store lower 32 bits of entry (address bits 31-22)
    or [edi], edx                 ; Combine address with flags
    add eax, 0x400000             ; Next 4MB physical address
    add edi, 4                    ; Move to next directory entry
    loop .set_entry

    ; Load page directory address into CR3
    mov eax, __page_dir
    mov cr3, eax

    mov eax, cr4
    or eax, CR4_PSE
    mov cr4, eax

    mov eax, cr0
    or eax, CR0_WP | CR0_PG
    mov cr0, eax

    ret
