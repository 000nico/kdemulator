# emu-stubs

Implements the `StubResolver` trait defined by `emu-core`. This is where the behavior of each imported NT function actually lives — `emu-core` only knows that a function was called and with what arguments; this crate decides what happens next.

## Responsibilities

**Native C++ implementations**
A catalog of NT API functions implemented directly in C++: `ExAllocatePoolWithTag`, `RtlCopyMemory`, `IoCreateDevice`, `KeInitializeSpinLock`, and others as they're added. This is expected to be the largest part of the crate in terms of raw line count, but it's a flat list of functions, not a complex subsystem — each stub reads arguments from `StubContext`, does something with emulated memory or engine state, and returns a value.

**Python fallback**
When a driver imports a function that has no C++ implementation, this crate looks for a matching Python file (for example, a `stubs/` directory the user points the CLI at) and, if found, starts an embedded Python interpreter to run it. Arguments and memory access are exposed to the Python side through the same `StubContext` abstraction used on the Rust side, so a Python stub and a Rust stub look similar from the driver's point of view.

**Resolution order**
C++ implementations are checked first. Python is only invoked as a fallback for functions with no native coverage, and the interpreter is only initialized the first time it's actually needed — a driver that only calls already-implemented functions never touches Python.

## Why this split exists

Two reasons:

* Performance: the small set of functions a driver calls frequently (allocation, memory copy, spinlocks) run as native code, without paying for a Python call on every import.
* Extensibility without recompilation: if a driver imports something obscure that isn't covered yet, a user can write a short Python function for that one case instead of having to modify and rebuild the Rust codebase.

## What it does not do

* Does not run driver code or manage CPU emulation — that's `emu-core`.
* Does not decide which driver to load, which IOCTLs to send, or how to run a fuzzing loop — that's `emu-cli`.
* Python involvement is scoped to individual function stubs, not to controlling the emulator as a whole.

