#pragma once
#include "../Api.hpp"
#include "../../../emu-core/src/debug/Debug.hpp"
#include "allocators_common.hpp"
#include <string>


class ApiExAllocatePool3 : public Api {
    public:
        uint64_t call(CPU* cpu) override {
            uint64_t arg0 = get_arg(cpu, 0);
            size_t arg1 = get_arg(cpu, 1);
            uint32_t arg2 = get_arg(cpu, 2);
            // we just ignorate arg3 and arg4
        
            bool isPaged = (arg0 & 0x02); // contains paginated bit?
            uint32_t simulatedPoolType = isPaged ? 1 : 0;
            
            if(isPaged)
                Debug::debug_msg("[ExAllocatePool3] Allocated paged memory, size = " + std::to_string(arg1), LOG_INFO);
            else
                Debug::debug_msg("[ExAllocatePool3] Allocated non-paged memory, size = " + 
                    std::to_string(arg1) +
                    " , Tag = " + std::to_string(arg2), LOG_INFO);

            return allocate(arg1, simulatedPoolType, arg2, cpu);
        }
};