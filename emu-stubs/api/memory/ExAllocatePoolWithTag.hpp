#pragma once
#include "../Api.hpp"
#include "../../../emu-core/src/debug/Debug.hpp"
#include "allocators_common.hpp"
#include <string>

class ApiExAllocatePoolWithTag : public Api {
    public:
        uint64_t call(CPU* cpu) override {
            uint32_t poolType = get_arg(cpu, 0); 
            size_t size = get_arg(cpu, 1);      
            uint32_t tag = get_arg(cpu, 2);      

            if(isPagedPool(poolType)) {
                Debug::debug_msg("[ExAllocatePoolWithTag] Allocated paged memory, size = " + 
                    std::to_string(size) + 
                    " , Tag = " + std::to_string(tag), LOG_INFO);
            } else {
                Debug::debug_msg("[ExAllocatePoolWithTag] Allocated non-paged memory, size = " + 
                    std::to_string(size) + 
                    " , Tag = " + std::to_string(tag), LOG_INFO);
            }

            return allocate(size, poolType, tag, cpu);
        }
};