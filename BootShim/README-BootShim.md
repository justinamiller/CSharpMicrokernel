# BootShim (offline starter)

This folder contains a **fully commented, offline starter** for the bare‑metal bootstrap:
- `boot/` — 32‑bit Multiboot2 entry + GDT + long‑mode transition
- `kernel/` — 64‑bit C kernel entry (`kmain`), VGA text console, IDT/PIC, keyboard driver, and a **stub** `ManagedMain()`
- `grub/` — GRUB 2 config to boot the ELF image
- `linker.ld` — link script placing sections at 1 MiB
- `Makefile` — build, ISO, and QEMU run targets

> ⚠️ **Important**: The identity‑mapped **page tables are TODO** (see `boot/longmode.s`). You must fill PML4/PDPT/PD with valid entries before enabling paging, or the CPU will triple‑fault. The code compiles as a scaffold so you can work offline and incrementally implement paging.

## Build (Linux/WSL)

Install tools:
```bash
sudo apt update
sudo apt install -y build-essential clang lld nasm make grub-pc-bin xorriso qemu-system-x86
```

Build and run:
```bash
make
make run
```

## Integrating C# (NativeAOT)

For offline work, `kernel/managed_stub.c` provides an empty `ManagedMain()` so you can build without .NET.

When ready to integrate C#:
1. Create a NativeAOT project that exports `[UnmanagedCallersOnly(EntryPoint="ManagedMain")]`.
2. Build it to a native object `.o` (or `.a`) and link it in place of `managed_stub.o`.
3. Call into your managed kernel loop from `kmain()`.

---

Happy hacking!
