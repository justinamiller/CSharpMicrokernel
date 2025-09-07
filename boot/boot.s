; boot/boot.s
; Purpose: Multiboot2 header + 32‑bit entry. Sets a temp stack and transfers
; control to routines that enable 64‑bit long mode, then jumps there.
; You start in 32‑bit protected mode because GRUB does that for you.

BITS 32

SECTION .multiboot
align 8
multiboot2_header:
    dd 0xe85250d6                ; Multiboot2 magic
    dd 0                         ; Architecture (0 = i386)
    dd header_end - multiboot2_header
    dd 0 - (0xe85250d6 + 0 + (header_end - multiboot2_header)) ; checksum
header_end:

SECTION .text
global _start
_start:
    cli                           ; disable interrupts until IDT is ready
    mov esp, stack_top            ; temporary 32‑bit stack

    extern load_gdt64
    call load_gdt64               ; install a GDT that includes 64‑bit code/data

    extern enable_long_mode_and_jump
    jmp enable_long_mode_and_jump ; continue CPU setup there

SECTION .bss
align 16
stack_bottom: resb 16384          ; 16 KiB temp stack
stack_top:
