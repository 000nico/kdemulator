#pragma once
#include <cstdint>
#include "../pe/format.hpp"
#include "../cpu/CPU.hpp"

void addTrapHookCallback(CPU* cpu);
void addCodeHookCallback(CPU* cpu, PE* pe);