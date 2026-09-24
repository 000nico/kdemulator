#pragma once
#include "../cpu/CPU.hpp"
#include "../pe/format.hpp"

void allocate_driver_object(CPU* cpu, PE* driver_pe);
void allocate_device_object(CPU* cpu);
void allocate_kuser(CPU* cpu);