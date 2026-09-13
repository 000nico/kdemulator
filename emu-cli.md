# emu-cli

The command-line executable. This is the only crate in the project that produces a binary and the only one with a `main` function; it doesn't implement emulation or stub logic itself, it wires the other two crates together.

## Responsibilities

**Argument parsing**
Takes the driver path, an optional stubs directory for Python fallbacks, and parameters for what to run (call `DriverEntry` only, send a single IOCTL, or run a fuzzing loop against a specific IOCTL code).

**Wiring**
Constructs a `DriverEmulator` from `emu-core`, builds a resolver from `emu-stubs`, and connects the two via `set_resolver`.

**Execution modes**
- Load-only: map the driver and call `DriverEntry`, report success/failure and any missing stub encountered along the way.
- Single IOCTL: send one IOCTL code with a given input buffer and print the result.
- Fuzzing loop: repeatedly mutate an input buffer, send it through the dispatch routine, and record crashes or anomalies.

**Reporting**
Prints or logs execution results: which imports were resolved and by what (C++ or Python), crash information when the emulator hits invalid memory access outside of a handled SEH scope, and basic coverage or timing information if enabled.

## What it does not do

- Does not implement any NT API behavior — that's `emu-stubs`.
- Does not implement PE mapping, memory management, or CPU emulation — that's `emu-core`.
- Does not embed emulator internals in its own logic; if something needs to change about how a function is emulated, it belongs in `emu-core` or `emu-stubs`, not here.
