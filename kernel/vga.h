// kernel/vga.h
// Purpose: Minimal direct VGA text buffer API for early console output.
// Writes to physical 0xB8000 assuming identity mapping.

#pragma once
#include <stdint.h>

void vga_clear();
void vga_write(const char* s);
