#pragma once
#include "../emu-core/src/cpu/CPU.hpp"
#include <cstdint>

class Api {
    public:
        virtual ~Api() {}
        virtual uint64_t call(CPU* cpu) = 0;

        void invoke(CPU* cpu) {
            uint64_t ret_val = call(cpu);

            // simulate ret (return), which is stored in rax
            cpu->set_register(REG_RAX, ret_val);

            // pop return addres from stack and jump
            uint64_t rsp = cpu->get_register(REG_RSP);
            uint64_t ret_addr = 0;
            cpu->mem_read(rsp, &ret_addr, sizeof(uint64_t));
            cpu->set_register(REG_RSP, rsp + 8);
            cpu->set_register(REG_RIP, ret_addr);
        }
        
    private:
        
};