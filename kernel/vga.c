// kernel/vga.c
// Purpose: Minimal VGA text writer (80x25) for early boot diagnostics.

#include "vga.h"

static volatile uint16_t* const VGA = (uint16_t*)0xB8000; // VGA text base
static int row = 0, col = 0;
static unsigned char attr = 0x07; // light grey on black

static void putc(char ch){
  if (ch == '\n') { row++; col = 0; }
  else {
    VGA[row*80 + col] = (uint16_t)attr<<8 | (unsigned char)ch;
    if (++col >= 80) { col = 0; row++; }
  }
  if (row >= 25) row = 24; // clamp (no scroll implemented)
}

void vga_clear(){
  for (int r=0;r<25;r++)
    for (int c=0;c<80;c++)
      VGA[r*80+c] = (uint16_t)attr<<8 | ' ';
  row = col = 0;
}

void vga_write(const char* s){
  while (*s) putc(*s++);
}
