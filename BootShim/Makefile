# Makefile (BootShim)
# Purpose: Assemble 32‑bit bootstrap, compile 64‑bit kernel C, and link an ELF.
# NOTE: This is a starter; paging tables in longmode.s must be filled before run.
# Build tools are assumed to be available (nasm, clang/ld.lld, grub-mkrescue, xorriso, qemu).

NASM := nasm
CC32 := clang
CC64 := clang
LD   := ld.lld

BUILD := build
OBJ := $(BUILD)/obj
BIN := $(BUILD)/bin

all: $(BIN)/kernel.elf iso

# ---------- 1) Assemble 32‑bit bootstrap ----------
$(OBJ)/boot.o: boot/boot.s
	@mkdir -p $(OBJ)
	$(NASM) -f elf32 $< -o $@

$(OBJ)/gdt64.o: boot/gdt64.s
	$(NASM) -f elf32 $< -o $@

$(OBJ)/longmode.o: boot/longmode.s
	$(NASM) -f elf32 $< -o $@

# ---------- 2) Compile kernel C (64‑bit) ----------
# We compile the kernel C files as 64‑bit, because after long mode switch we call kmain.
$(OBJ)/kmain.o: kernel/kmain.c kernel/vga.h kernel/idt.h
	$(CC64) -ffreestanding -fno-stack-protector -m64 -O2 -c $< -o $@

$(OBJ)/vga.o: kernel/vga.c kernel/vga.h
	$(CC64) -ffreestanding -fno-stack-protector -m64 -O2 -c $< -o $@

$(OBJ)/idt.o: kernel/idt.c kernel/idt.h
	$(CC64) -ffreestanding -fno-stack-protector -m64 -O2 -c $< -o $@

$(OBJ)/keyboard.o: kernel/keyboard.c kernel/idt.h
	$(CC64) -ffreestanding -fno-stack-protector -m64 -O2 -c $< -o $@

$(OBJ)/managed_stub.o: kernel/managed_stub.c
	$(CC64) -ffreestanding -fno-stack-protector -m64 -O2 -c $< -o $@

# ---------- 3) Link kernel ----------
# We use a 32‑bit multiboot entry and then execute 64‑bit code.
# lld will link mixed‑mode objects as long as relocations resolve; however,
# some linkers may complain. If so, consider building a 2‑stage boot (32‑>64) or Limine.
$(BIN)/kernel.elf: $(OBJ)/boot.o $(OBJ)/gdt64.o $(OBJ)/longmode.o $(OBJ)/kmain.o $(OBJ)/vga.o $(OBJ)/idt.o $(OBJ)/keyboard.o $(OBJ)/managed_stub.o linker.ld
	@mkdir -p $(BIN)
	$(LD) -T linker.ld -nostdlib -o $@ $^

# ---------- 4) ISO (GRUB) ----------
iso: $(BIN)/kernel.elf grub/grub.cfg
	@mkdir -p $(BUILD)/iso/boot/grub
	cp $(BIN)/kernel.elf $(BUILD)/iso/boot/kernel.elf
	cp grub/grub.cfg $(BUILD)/iso/boot/grub/grub.cfg
	grub-mkrescue -o $(BUILD)/csharp-microkernel.iso $(BUILD)/iso

# ---------- 5) Run in QEMU ----------
run: iso
	qemu-system-x86_64 -cdrom $(BUILD)/csharp-microkernel.iso -serial stdio

clean:
	rm -rf $(BUILD)
