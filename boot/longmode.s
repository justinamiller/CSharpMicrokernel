; boot/longmode.s
; Purpose: Turn on PAE + long mode + paging, jump to 64‑bit entry, set 64‑bit stack,
; then call kmain (written in C). NOTE: you must supply real identity‑mapped page
; tables in pml4/pdpt/pd before enabling paging or the CPU will triple‑fault.

BITS 32

SECTION .text
global enable_long_mode_and_jump
extern kmain                 ; C entry (will execute in 64‑bit mode)
extern pml4                  ; paging root (identity map needed)

enable_long_mode_and_jump:
    ; Enable LME via IA32_EFER MSR
    mov ecx, 0xC0000080
    rdmsr
    or eax, 1 << 8          ; LME bit
    wrmsr

    ; Enable PAE + PGE
    mov eax, cr4
    or eax, (1 << 5) | (1 << 7)
    mov cr4, eax

    ; Load PML4 base
    mov eax, pml4
    mov cr3, eax

    ; Enable paging (PG) and keep PE set
    mov eax, cr0
    or eax, 0x80000001      ; PG + PE
    mov cr0, eax

    ; Far jump into 64‑bit code segment selector 0x08
    jmp 0x08:long_mode_entry

[BITS 64]
long_mode_entry:
    mov ax, 0x10            ; data selector
    mov ds, ax
    mov es, ax
    mov ss, ax

    lea rsp, [rel stack64_top] ; 64‑bit stack

    call kmain              ; into C

.hang:
    hlt
    jmp .hang

SECTION .bss
align 16
stack64: resb 32768         ; 32 KiB 64‑bit stack
stack64_top:

SECTION .data
align 4096
global pml4
pml4:
    ; TODO: Fill PML4/PDPT/PD entries for identity mapping.
    ; This placeholder is here so the file assembles. Without real entries,
    ; enabling paging will fault.
    times 512 dq 0
