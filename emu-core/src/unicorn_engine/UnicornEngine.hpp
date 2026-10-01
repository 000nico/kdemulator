#pragma once

#include "hook.hpp"
#include "../cpu/CPU.hpp"

#include <vector>
#include <cstdint>
#include <unicorn/unicorn.h>

class UnicornEngine : public CPU {
    public:
        UnicornEngine();
        ~UnicornEngine();

        // registers
        uint64_t get_register(Register reg);
        bool set_register(Register reg, uint64_t value);
        
        // stack
        uintptr_t get_stack_base();
        size_t get_stack_size();

        // heap
        uintptr_t get_heap_base();
        size_t get_heap_size();

        // memory
        bool mem_map(uintptr_t address, size_t size, uint32_t perms);
        bool mem_write(uintptr_t address, void* data, size_t size);
        bool mem_read(uintptr_t address, void* buffer, size_t size);
        bool apply_mem_prot(uintptr_t address, size_t size, uint32_t perms);

        // perms
        int to_uc_prot(uint32_t prot);

        // hook
        bool add_code_hook(uint64_t begin, uint64_t end, CodeHookFn callback, void* user_data);
        
        // execution
        CpuStatus step_cpu();
        bool start(uintptr_t address);
        void stop();
    
    private:
        uc_engine* uc;
        std::vector<HookContext*> hooks;
};