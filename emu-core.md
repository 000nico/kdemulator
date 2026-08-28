# emu-core

The emulation engine. This crate has no knowledge of Python or any scripting layer — it only defines an interface for resolving imported functions and expects something else (`emu-stubs`) to implement it.

## Responsibilities

**PE loading**
Parses the target `.sys` file and maps its sections into emulated memory, respecting each section's virtual address, size, and permissions (as opposed to its on-disk layout). Applies base relocations from the `.reloc` section when the mapped address doesn't match the image's preferred base.

**Unicorn wrapper**
Initializes the CPU context (x86-64), sets up the stack, and drives execution. Provides the memory read/write/map primitives that the rest of the crate and `emu-stubs` build on.

**Import resolution hook**
Rewrites the driver's IAT so that each imported function points to a trap address instead of real code. When execution hits one of these addresses, the engine stops, identifies which function was called, and delegates to an external resolver — it does not decide what the function does.

**Kernel structs**
Provides minimal, fields-as-needed versions of the structures a driver directly touches during load and execution: `DRIVER_OBJECT`, `DEVICE_OBJECT`, `IRP`, `IO_STACK_LOCATION`. These aren't full reimplementations of the real structs — only the fields a driver is expected to read or write are populated.

**IRQL tracking**
Keeps a current IRQL value and exposes it so that stub implementations can check whether an operation is valid at the current level (e.g. touching paged memory at `DISPATCH_LEVEL`).

**SEH scope tracking**
Maintains a stack of active `__try` regions so that a memory access violation during emulation can be redirected to the corresponding `__except` handler instead of aborting execution outright, approximating how the real kernel handles guarded access to user buffers.

**Pool allocator**
A simple bump/free-list allocator over a reserved region of emulated memory, used to back pool allocation stubs (`ExAllocatePool*`) with real addresses that can be read and written.

## What it does not do

- Does not implement any specific NT API function's behavior — that's `emu-stubs`.
- Does not know whether a resolver is written in Rust or backed by Python.
- Does not parse command-line arguments or manage a fuzzing loop — that's `emu-cli`.

## Public interface (conceptual)

The crate exposes a `DriverEmulator` type and a resolver trait, roughly:

```rust
trait StubResolver {
    fn resolve(&mut self, name: &str, ctx: &mut StubContext) -> u64;
}

impl DriverEmulator {
    fn new(path: &str) -> Result<Self>;
    fn load_driver(&mut self) -> Result<()>;
    fn set_resolver(&mut self, resolver: impl StubResolver);
    fn call_driver_entry(&mut self) -> Result<u32>;
    fn send_ioctl(&mut self, code: u32, input: &[u8]) -> Result<Vec<u8>>;
}
```

Anything implementing `StubResolver` can be plugged in; `emu-core` doesn't care what's on the other side of that trait.
