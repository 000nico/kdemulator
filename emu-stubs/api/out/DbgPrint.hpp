#pragma once
#include "../Api.hpp"
#include "../../../emu-core/src/debug/Debug.hpp"

class ApiDbgPrint : public Api {
    public:
        uint64_t call(CPU* cpu) override {
            std::string msg = read_string(cpu, get_arg(cpu, 0));
            Debug::debug_msg("[DbgPrint] " + msg, LOG_INFO);
            return 0; 
        }
};