#pragma once
#include "../Api.hpp"
#include "../../../emu-core/src/debug/Debug.hpp"
#include "allocators_common.hpp"
#include <string>

class ApiExFreePool : public Api {
    public:
        uint64_t call(CPU* cpu) override {
            uint64_t addr  = get_arg(cpu, 0); 

            Debug::debug_msg("[ExFreePool] addr = " + std::to_string(addr), LOG_INFO);
            
            free_mem(addr);
            
            return 0; // actually does not return but i think im safe
        }
};