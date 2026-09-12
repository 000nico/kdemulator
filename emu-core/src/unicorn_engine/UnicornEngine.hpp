#pragma once
#include "../cpu/cpu.hpp"
#include <cstdint>
#include <unicorn/unicorn.h>

class UnicornEngine : public CPU {
    public:
        UnicornEngine();
        ~UnicornEngine();

        // registers
        int get_register(Register reg);
        bool set_register(Register reg, uint64_t value);
        
        // stack
        uintptr_t get_stack_base() = 0;
        size_t get_stack_size() = 0;

        // heap
        uintptr_t get_heap_base() = 0;
        size_t get_heap_size() = 0;

        // memory
        bool mem_map(uintptr_t address, size_t size, uint32_t perms) = 0;
        bool mem_write(uintptr_t address, void* data, size_t size) = 0;
        bool mem_read(uintptr_t address, void* buffer, size_t size) = 0;
        bool apply_mem_prot(uintptr_t address, size_t size, uint32_t perms) = 0;

        // perms
        int to_uc_prot(uint32_t prot);
        
        // execution
        CpuStatus step_cpu() = 0;
    
        bool start();
    
    private:
        uc_engine* uc;
};