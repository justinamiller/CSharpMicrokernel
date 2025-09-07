// kernel/kmain.c
// Purpose: First C code executed after switching to 64‑bit mode.
// Initializes IDT/PIC, clears VGA, then calls into ManagedMain (C# or stub).

#include <stdint.h>
#include "vga.h"
#include "idt.h"

typedef void (*managed_entry_t)(void);
extern void ManagedMain(void); // provided by NativeAOT later (stubbed for now)

void kmain(void) {
  vga_clear();
  vga_write("Booting C# microkernel (BootShim)...\n");

  pic_remap_and_unmask_kbd();
  idt_init();
  vga_write("IDT/PIC ready. Handing off to ManagedMain...\n");

  ((managed_entry_t)ManagedMain)(); // will return immediately with the stub

  vga_write("ManagedMain returned. Halting.\n");
  for(;;) __asm__ volatile("hlt");
}
