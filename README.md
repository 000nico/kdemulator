<div align="center">

<img src="assets/banner.jpg" alt="blue-pill" width="600">

<br><br>

[![license](https://img.shields.io/badge/license-MIT-blue?style=flat-square)](LICENSE)
[![language](https://img.shields.io/badge/language-C%2B%2B-00599C?style=flat-square&logo=c%2B%2B&logoColor=white)](https://isocpp.org/)
[![platform](https://img.shields.io/badge/platform-Windows-0078D6?style=flat-square&logo=windows&logoColor=white)](https://www.microsoft.com)

**[features](#features) · [docs](docs/) · [contribute](#contribute)**

*kernel driver emulator.*

</div>


# kdemulator

A CPU-level emulator for Windows kernel drivers (`.sys` files), built on top of the Unicorn Engine. The goal is to load a driver, run its `DriverEntry` and IOCTL dispatch routines outside of a real Windows kernel, and use the result as a target for fuzzing or manual analysis.

## What it does

Windows drivers are PE images that import functions from `ntoskrnl.exe`, `hal.dll`, and other drivers. This project maps a driver's code into an emulated address space, executes its real x86-64 instructions with Unicorn, and intercepts every call to an imported kernel function. Each intercepted call is resolved to an implementation that simulates what that function would do, without requiring a full Windows kernel or a virtual machine.

The emulator does not model a complete Windows kernel. It models only what a given driver actually touches: the subset of the NT API it imports, a minimal `DRIVER\_OBJECT`/`DEVICE\_OBJECT`/`IRP` layout, and enough memory/IRQL bookkeeping to run the driver's code path without crashing on missing state.

## Why

Running a real driver against a real Windows kernel for fuzzing requires a VM, snapshotting, and reset cycles that are slow and hard to parallelize. Emulating the driver directly is faster to iterate on and easier to instrument (memory access, register state, and crash conditions are all directly inspectable), at the cost of fidelity: anything the emulator doesn't model correctly can produce false positives or false negatives.

## Structure

The project is split into three crates, each documented separately:

* [`emu-core`](./emu-core.md) — the emulation engine: PE mapping, Unicorn wrapper, kernel structs, IRQL/SEH bookkeeping.
* [`emu-stubs`](./emu-stubs.md) — implementations of NT API functions, in Rust by default, with an optional Python fallback for functions not covered natively.
* [`emu-cli`](./emu-cli.md) — the command-line executable that ties the above together.

## Design principle: C++ first, Python as fallback

Common NT API functions (`ExAllocatePoolWithTag`, `RtlCopyMemory`, `IoCreateDevice`, etc.) are implemented natively in C++ for speed and to keep the tool usable without any scripting setup. When a driver imports a function that isn't covered, instead of requiring a C++ change and a recompile, a user can drop in a Python file implementing just that function. The emulator only starts a Python interpreter if a stub actually falls back to it; drivers that only use already-covered functions never touch Python.

## Known limitations

* Only the driver's own code and its directly imported functions are emulated. There is no scheduler, no real device stack, no other drivers unless explicitly stubbed.
* IRQL and SEH are modeled at a basic level; they catch the common cases but are not a full reimplementation of kernel exception handling.
* Hardware-facing drivers (MMIO, port I/O, real PCI config space) require additional stubs not covered by default.
* Fidelity is scoped to what's needed for IOCTL-level analysis and fuzzing, not general-purpose kernel emulation.

## Why am i doing this

God had no hand in the creation of this abhorrence. The fact that this monolith exists proves that god is either impotent to alter his universe or ignorant to the horrors taking place in his Kingdom

