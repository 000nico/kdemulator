#pragma once
#include "../emu-core/src/cpu/CPU.hpp"
#include <cstdint>
#include <string>

class Api {
    public:
        virtual ~Api() {}
        virtual uint64_t call(CPU* cpu) = 0;
        
    protected:
        static uint64_t get_arg(CPU* cpu, int index);
        static std::string read_string(CPU* cpu, uint64_t address);
        static std::string read_unicode_string(CPU* cpu, uint64_t unicode_str_addr);
        static std::string read_ansi_string(CPU* cpu, uint64_t ansi_str_addr);
        static std::wstring read_wide_string(CPU* cpu, uint64_t address, size_t max_chars = 260);
        static uint32_t read_u32(CPU* cpu, uint64_t address);
        static uint64_t read_u64(CPU* cpu, uint64_t address);
        static bool write_u32(CPU* cpu, uint64_t address, uint32_t val);
        static bool write_u64(CPU* cpu, uint64_t address, uint64_t val);
};