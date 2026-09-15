#pragma once
#include "../Api.hpp"
#include "../../../emu-core/src/debug/Debug.hpp"
#include "allocators_common.hpp"

class ApiExAllocatePool : public Api {
    public:
        uint64_t call(CPU* cpu) override {
            uint32_t arg0 = get_arg(cpu, 0);
            size_t arg1 = get_arg(cpu, 1);

            if(isPagedPool(arg0))
                Debug::debug_msg("[ExAllocatePool] Allocated paged memory, size = " + std::to_string(arg1), LOG_INFO);
            else
                Debug::debug_msg("[ExAllocatePool] Allocated non-paged memory, size = " + std::to_string(arg1), LOG_INFO);;

            return allocate(arg1, arg0, 0, cpu);; 
        }
};