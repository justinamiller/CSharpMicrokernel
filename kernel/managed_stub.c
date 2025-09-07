// kernel/managed_stub.c
// Purpose: Temporary stub so you can build BootShim offline without C# NativeAOT.
// Replace this with a NativeAOT export: [UnmanagedCallersOnly(EntryPoint="ManagedMain")]

void ManagedMain(void) {
  // No‑op: when you add your C# NativeAOT module, this symbol will be provided there.
}
