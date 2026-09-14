#pragma once
#include "../Api.hpp"
#include "../../../emu-core/src/debug/Debug.hpp"

class ApiDbgPrintEx : public Api {
    public:
        uint64_t call(CPU* cpu) override {
            std::string msg = read_string(cpu, get_arg(cpu, 2));
            Debug::debug_msg("[DbgPrintEx] " + msg, LOG_INFO);
            return 0; 
        }
};