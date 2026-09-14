#pragma once
#include "format.hpp"
#include "TrapEntry.hpp"
#include "../cpu/CPU.hpp"

class PEManager {
    public:
        bool load_pe(CPU* cpu, PE pe, uintptr_t address, std::vector<TrapEntry>* trap_table = nullptr);
        bool read_pe_from_disk(char* path, PE* pe);
        
    private:
        
};
