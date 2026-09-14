#pragma once
#include "../PEManager.hpp"
#include "../format.hpp"
#include "../TrapEntry.hpp"
#include "../../cpu/CPU.hpp"
#include <hex.hpp>

static inline uint64_t align_up(uint64_t value, uint64_t alignment) {
    return (value + alignment - 1) & ~(alignment - 1);
}

bool load_headers(CPU* cpu, PE pe, uintptr_t address);
bool load_sections(CPU* cpu, PE pe, uintptr_t address);
bool resolve_reloc(CPU* cpu, PE pe, uintptr_t address);
bool resolveIat(CPU* cpu, PE pe, uintptr_t address, std::vector<TrapEntry>* trap_table);
bool resolveIatWithHookTrap(CPU* cpu, PE pe, uintptr_t address, std::vector<TrapEntry>* trap_table);
uint32_t section_to_prot(uint32_t characteristics);
bool applyMemProtect(CPU* cpu, PE pe, uintptr_t address);
