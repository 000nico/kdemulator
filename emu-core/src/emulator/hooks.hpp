#pragma once
#include <cstdint>
#include "../pe/format.hpp"
#include "../cpu/CPU.hpp"
#include "../emu-stubs/APIDispatcher/APIDispatcher.hpp"

void addTrapHookCallback(CPU* cpu, APIDispatcher* dispatcher);
void addCodeHookCallback(CPU* cpu, PE* pe);