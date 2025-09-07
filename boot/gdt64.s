; boot/gdt64.s
; Purpose: Load a minimal GDT with 64‑bit code + data descriptors.
; Required even in long mode to satisfy segment regs.

BITS 32

SECTION .text
global load_gdt64
load_gdt64:
    lgdt [gdt_descriptor]
    ret

SECTION .rodata
gdt:
    dq 0x0000000000000000   ; null
    dq 0x00AF9A000000FFFF   ; code: base=0, limit=FFFFF, 4K gran, exec/read
    dq 0x00AF92000000FFFF   ; data: base=0, limit=FFFFF, 4K gran, read/write

gdt_descriptor:
    dw gdt_end - gdt - 1
    dd gdt
gdt_end:
