#pragma once
#include "../pe/format.hpp"
#include "../cpu/CPU.hpp"
#include "../emu-stubs/APIDispatcher/APIDispatcher.hpp"
#include "../disasm/Disasm.hpp"

void addTrapHookCallback(CPU* cpu, APIDispatcher* dispatcher);
void addCodeHookCallback(CPU* cpu, PE* pe, Disasm* disasm = nullptr);