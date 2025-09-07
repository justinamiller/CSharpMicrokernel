/* linker.ld
   Purpose: Link kernel as a flat ELF image starting at 1 MiB.
   NOTE: This script assumes a multiboot 32‑bit entry at _start in .text.
*/
ENTRY(_start)
SECTIONS
{
  . = 1M;

  .text : { *(.multiboot) *(.text*) }
  .rodata : { *(.rodata*) }
  .data : { *(.data*) }
  .bss : { *(.bss*) *(COMMON) }
}
