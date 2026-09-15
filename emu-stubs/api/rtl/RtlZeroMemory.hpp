#pragma once
#include "../Api.hpp"
#include "../../../emu-core/src/debug/Debug.hpp"
#include <string>

class ApiRtlZeroMemory : public Api {
    public:
        uint64_t call(CPU* cpu) override {
            uint64_t destination = get_arg(cpu, 0);
            size_t lenght = get_arg(cpu, 1);

            std::vector<uint8_t> zeros(lenght, 0);
            cpu->mem_write(destination, zeros.data(), lenght);
            
            Debug::debug_msg("[RtlZeroMemory] pointer = " + std::to_string(destination) + ", lenght = " + std::to_string(lenght), LOG_INFO);
            return 0;
        }
};