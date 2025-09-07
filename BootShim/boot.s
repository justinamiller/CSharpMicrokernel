; boot/boot.s
; Purpose: Provide a Multiboot2 header so GRUB can load us, and define _start.
; We begin in 32-bit protected mode (set by GRUB), set up a temporary stack,
; and jump into helper routines to prepare 64-bit long mode.

BITS 32

SECTION .multiboot
align 8
multiboot2_header:
    dd 0xe85250d6                ; Multiboot2 magic (required)
    dd 0                         ; Architecture (0 = i386)
    dd header_end - multiboot2_header ; Total header length
    dd 0 - (0xe85250d6 + 0 + (header_end - multiboot2_header))
                                   ; Checksum: magic + arch + len + checksum == 0
header_end:

SECTION .text
global _start
_start:
    cli                           ; Disable interrupts until we’re ready
    mov esp, stack_top            ; Set a temporary 32-bit stack

    extern load_gdt64             ; Load a 64-bit capable GDT (still in 32-bit mode)
    call load_gdt64

    extern enable_long_mode_and_jump
    jmp enable_long_mode_and_jump ; Continue CPU setup (paging + long mode) there

; Simple 16 KiB temporary stack while in 32-bit mode.
SECTION .bss
align 16
stack_bottom: resb 16384
stack_top:
