#pragma once
#include "../emu-core/src/cpu/CPU.hpp"
#include <cstdint>

class Api {
    public:
        virtual ~Api() {}
        virtual uint64_t call(CPU* cpu) = 0;
        
    protected:
        static uint64_t get_arg(CPU* cpu, int index);
        static std::string read_string(CPU* cpu, uint64_t address);
        
};