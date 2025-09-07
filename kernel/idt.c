// kernel/idt.c
// Purpose: Install a tiny 64‑bit IDT and remap the legacy PIC so IRQ1 -> 0x21.

#include "idt.h"

static inline void outb(uint16_t port, uint8_t val) {
  __asm__ volatile("outb %0,%1"::"a"(val),"Nd"(port));
}
static inline uint8_t inb(uint16_t port) {
  uint8_t v; __asm__ volatile("inb %1,%0":"=a"(v):"Nd"(port));
  return v;
}

static idt_entry_t g_idt[256];
static idt_ptr_t   g_idtp;

extern void isr_stub_irq1();

static void lidt(idt_ptr_t* p){ __asm__ volatile("lidt (%0)"::"r"(p)); }

void idt_set_gate(int vec, void* isr) {
  uint64_t addr = (uint64_t)isr;
  g_idt[vec].offset_low  = (uint16_t)(addr & 0xFFFF);
  g_idt[vec].selector    = 0x08;      // must match GDT code selector
  g_idt[vec].ist         = 0;
  g_idt[vec].type_attr   = 0x8E;      // present, DPL=0, interrupt gate
  g_idt[vec].offset_mid  = (uint16_t)((addr >> 16) & 0xFFFF);
  g_idt[vec].offset_high = (uint32_t)((addr >> 32) & 0xFFFFFFFF);
  g_idt[vec].zero        = 0;
}

void pic_remap_and_unmask_kbd(void) {
  const uint16_t PIC1_CMD=0x20, PIC1_DATA=0x21, PIC2_CMD=0xA0, PIC2_DATA=0xA1;
  const uint8_t ICW1_INIT=0x10, ICW1_ICW4=0x01, ICW4_8086=0x01;

  uint8_t a1 = inb(PIC1_DATA), a2 = inb(PIC2_DATA);

  outb(PIC1_CMD, ICW1_INIT | ICW1_ICW4);
  outb(PIC2_CMD, ICW1_INIT | ICW1_ICW4);

  outb(PIC1_DATA, 0x20); // master offset
  outb(PIC2_DATA, 0x28); // slave offset

  outb(PIC1_DATA, 4);    // tell master there is a slave at IRQ2
  outb(PIC2_DATA, 2);    // tell slave its cascade id

  outb(PIC1_DATA, ICW4_8086);
  outb(PIC2_DATA, ICW4_8086);

  // Unmask IRQ1 (keyboard) and IRQ2 (cascade)
  uint8_t new_a1 = (a1 & ~(1<<1)) & ~(1<<2);
  outb(PIC1_DATA, new_a1);
  outb(PIC2_DATA, a2);
}

void idt_init(void) {
  for (int i=0;i<256;i++){
    g_idt[i].offset_low=0; g_idt[i].selector=0; g_idt[i].ist=0;
    g_idt[i].type_attr=0; g_idt[i].offset_mid=0; g_idt[i].offset_high=0; g_idt[i].zero=0;
  }
  idt_set_gate(0x21, isr_stub_irq1);    // keyboard
  g_idtp.base  = (uint64_t)&g_idt[0];
  g_idtp.limit = (uint16_t)(sizeof(g_idt)-1);
  lidt(&g_idtp);
  __asm__ volatile("sti");              // enable interrupts
}
