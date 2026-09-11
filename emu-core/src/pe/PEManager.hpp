#pragma once
#include "format.hpp"
#include "../cpu/CPU.hpp"

class PEManager {
    public:
        bool load_pe(CPU* cpu, PE pe, uintptr_t address);
        bool read_pe_from_disk(char* path, PE* pe);
        
    private:
        
};
