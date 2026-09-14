#include "Api.hpp"
#include <cstdint>
#include <string>

uint64_t Api::get_arg(CPU* cpu, int index){
    switch(index) {
        case 0: return cpu->get_register(REG_RCX);
        case 1: return cpu->get_register(REG_RDX);
        case 2: return cpu->get_register(REG_R8);
        case 3: return cpu->get_register(REG_R9);
        default: {
            uint64_t rsp = cpu->get_register(REG_RSP);
            uint64_t val = 0;
            cpu->mem_read(rsp + 0x20 + (index - 4) * 8, &val, sizeof(uint64_t));
            return val;
        }
    }
}

std::string Api::read_string(CPU* cpu, uint64_t address) {
    char buf[512] = {};
    cpu->mem_read(address, buf, sizeof(buf) - 1);
    return std::string(buf);
}