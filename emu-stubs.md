# emu-stubs

Implements the `ApiDispatcher` trait defined by `emu-core`. This is where the behavior of each imported NT function actually lives — `emu-core` only knows that a function was called and with what arguments; this crate decides what happens next.

## Responsibilities

**Native C++ implementations**
A catalog of NT API functions implemented directly in C++: `ExAllocatePoolWithTag`, `RtlCopyMemory`, `IoCreateDevice`, `KeInitializeSpinLock`, and others as they're added. This is expected to be the largest part of the crate in terms of raw line count, but it's a flat list of functions, not a complex subsystem — each stub reads arguments from `StubContext`, does something with emulated memory or engine state, and returns a value.

## What it does not do

* Does not run driver code or manage CPU emulation — that's `emu-core`.
* Does not decide which driver to load, which IOCTLs to send, or how to run a fuzzing loop — that's `emu-cli`.
* Python involvement is scoped to individual function stubs, not to controlling the emulator as a whole.

