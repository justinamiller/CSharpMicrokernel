// kernel/keyboard.c
// Purpose: Handle PS/2 keyboard IRQ1 and push ASCII chars into a ring buffer.
// Minimal scancode set 1 map, no shift/ctrl handling for brevity.

#include "idt.h"
#include <stdint.h>

#define KBD_DATA 0x60
#define PIC1_CMD 0x20
#define EOI      0x20

static inline uint8_t inb(uint16_t p){ uint8_t v; __asm__ volatile("inb %1,%0":"=a"(v):"Nd"(p)); return v; }
static inline void outb(uint16_t p,uint8_t v){ __asm__ volatile("outb %0,%1"::"a"(v),"Nd"(p)); }

#define KBUF_SZ 128
static volatile char kbuf[KBUF_SZ];
static volatile uint32_t khead=0, ktail=0;

static const char scancode_map[128] = {
  0,27,'1','2','3','4','5','6','7','8','9','0','-','=', '\b',
  '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n', 0,
  'a','s','d','f','g','h','j','k','l',';','\'','`',  0,'\\',
  'z','x','c','v','b','n','m',',','.','/',  0,'*',  0,' ',  0,
  /* rest unused */ 0
};

void kbd_isr_handler(void) {
  uint8_t code = inb(KBD_DATA);
  if (code < 128) {
    char ch = scancode_map[code];
    if (ch) {
      uint32_t next = (khead + 1) % KBUF_SZ;
      if (next != ktail) { kbuf[khead] = ch; khead = next; } // drop if full
    }
  }
  outb(PIC1_CMD, EOI);
}

int kbd_read_char(void) {
  if (ktail == khead) return -1;
  char ch = kbuf[ktail];
  ktail = (ktail + 1) % KBUF_SZ;
  return (int)(unsigned char)ch;
}

// Naked stub: saves regs, calls C handler, iretq back to interrupted code.
__attribute__((naked)) void isr_stub_irq1() {
  __asm__ volatile(
    "pushq %rax; pushq %rbx; pushq %rcx; pushq %rdx; pushq %rsi; pushq %rdi; pushq %rbp; pushq %r8; pushq %r9; pushq %r10; pushq %r11; pushq %r12; pushq %r13; pushq %r14; pushq %r15;"
    "call kbd_isr_handler;"
    "popq %r15; popq %r14; popq %r13; popq %r12; popq %r11; popq %r10; popq %r9; popq %r8; popq %rbp; popq %rdi; popq %rsi; popq %rdx; popq %rcx; popq %rbx; popq %rax;"
    "iretq"
  );
}
