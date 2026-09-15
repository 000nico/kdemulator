#pragma once
#include "../Api.hpp"
#include "../../../emu-core/src/debug/Debug.hpp"
#include <string>
#include <vector> 

class ApiRtlCopyMemory : public Api {
    public:
        uint64_t call(CPU* cpu) override {
            uint64_t destination = get_arg(cpu, 0);
            uint64_t source = get_arg(cpu, 1);
            size_t length = get_arg(cpu, 2);

            std::vector<uint8_t> buffer(length);

            cpu->mem_read(source, buffer.data(), length);
            cpu->mem_write(destination, buffer.data(), length);
            
            Debug::debug_msg("[RtlCopyMemory] dest = " + std::to_string(destination) + 
                             ", source = " + std::to_string(source) + 
                             ", length = " + std::to_string(length), LOG_INFO);
            return 0;
        }
};