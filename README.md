# CSharpMicrokernel

**CSharpMicrokernel** is an experimental **microkernel operating
system** written primarily in **C# (NativeAOT)**, supported by a minimal
**assembly + C bootstrap layer** ("BootShim"). It is designed to run on
bare-metal x86_64 hardware with **no runtime dependencies**, showcasing
how modern managed languages can drive systems-level development.

------------------------------------------------------------------------

## Vision

The project explores the fusion of **managed language ergonomics** with
**low-level OS engineering**. By leveraging .NET NativeAOT, we
achieve: - **Zero runtime dependencies on the target** --- no libc, no
CoreCLR, no operating system. - **Bare-metal execution**: runs directly
on hardware or emulators. - **Microkernel
architecture**: only essential primitives in kernel space (scheduling,
IPC, memory management), while services (console, FS, drivers) live in
isolated modules.

This is not just a proof of concept --- it is a platform to experiment
with **modern language design in OS construction**.

------------------------------------------------------------------------

## Architecture Overview

    ┌───────────────┐
    │   BootShim    │   Assembly + C
    │  (boot.s)     │   CPU bring-up, GDT/IDT, paging, stack
    └───────┬───────┘
            │
    ┌───────▼────────┐
    │    Kernel C    │   kmain.c, VGA, IDT, Keyboard, PIT
    └───────┬────────┘
            │
    ┌───────▼────────┐
    │  ManagedCore   │   C# (NativeAOT)
    │  Entry.cs      │   Scheduler, IPC, Shell, FS
    └────────────────┘

-   **BootShim**: Responsible for entering 64-bit long mode, building
    page tables, and handing off to C.
-   **C Layer**: Initializes interrupts (IDT/PIC), timers, and hardware
    drivers. Bridges unmanaged world to managed world.
-   **ManagedCore (C#)**: Provides the microkernel's heart ---
    scheduler, interprocess communication (IPC), shell, and higher-level
    services.

------------------------------------------------------------------------

## Current Features

-   ✅ Multiboot2-compliant boot via GRUB\
-   ✅ 64-bit long mode bring-up (assembly)\
-   ✅ VGA text console output (C + C#)\
-   ✅ IDT setup + PIC remap\
-   ✅ PS/2 keyboard driver (IRQ1) feeding into C# shell\
-   ✅ Interactive shell in C# with commands (`help`, `cls`, `echo`)

------------------------------------------------------------------------

## Roadmap

-   [ ] PIT timer + uptime counter\
-   [ ] Cooperative → preemptive scheduler\
-   [ ] Virtual memory manager with per-process address spaces\
-   [ ] IPC (send/recv/reply) channels\
-   [ ] Virtual File System (VFS) with RAMFS backend\
-   [ ] Disk drivers + FAT32 filesystem\
-   [ ] Networking (e1000 NIC driver + TCP/IP stack)\
-   [ ] User-mode processes and isolation

------------------------------------------------------------------------

## Build Instructions

### Prerequisites

-   **Windows 10/11 with Visual Studio 2022**

    -   Workloads: *Desktop Development with C++*, *.NET desktop
        development*

-   **WSL2 (Ubuntu 22.04)** (recommended) with:

    ``` bash
    sudo apt update
    sudo apt install build-essential clang lld nasm make grub-pc-bin xorriso qemu-system-x86 dotnet-sdk-8.0
    ```

### Build

``` bash
# In WSL or Linux
make all
```

### Run

``` bash
make run
```

This will launch QEMU and boot the kernel ISO.

------------------------------------------------------------------------

## 📂 Repository Structure

    CSharpMicrokernel/
    ├─ BootShim/          # Assembly + C bootstrap
    │  ├─ boot/           # Assembly files (boot.s, gdt64.s, longmode.s)
    │  ├─ kernel/         # C files (kmain.c, vga.c, idt.c, keyboard.c)
    │  ├─ fs/             # Future: filesystem support
    │  ├─ drivers/        # Future: hardware drivers
    │  ├─ grub/           # grub.cfg
    │  ├─ linker.ld       # Linker script
    │  └─ Makefile
    │
    ├─ ManagedCore/       # C# NativeAOT kernel logic
    │  ├─ Kernel.csproj
    │  ├─ Entry.cs
    │  ├─ Vga.cs
    │  ├─ Keyboard.cs
    │  ├─ Shell.cs
    │  └─ rd.xml
    │
    ├─ build/             # Generated build outputs
    └─ README.md

------------------------------------------------------------------------

