#pragma once
#include "registers.hpp"
#include "status.hpp"
#include <vector>
#include "../pe/TrapEntry.hpp"
#include <cstdint>

class CPU;

using CodeHookFn = void(*)(CPU* cpu, uint64_t address, void* user_data);

class CPU {
    public:
        virtual ~CPU() {}

        // registers
        virtual bool set_register(Register reg, uint64_t value) = 0;
        virtual uint64_t get_register(Register reg) = 0;

        // stack
        virtual uintptr_t get_stack_base() = 0;
        virtual size_t get_stack_size() = 0;

        // heap
        virtual uintptr_t get_heap_base() = 0;
        virtual size_t get_heap_size() = 0;

        // memory
        virtual bool mem_map(uintptr_t address, size_t size, uint32_t perms) = 0; 
        virtual bool mem_write(uintptr_t address, void* data, size_t size) = 0;
        virtual bool mem_read(uintptr_t address, void* buffer, size_t size) = 0;
        virtual bool apply_mem_prot(uintptr_t address, size_t size, uint32_t perms) = 0;

        // cpu
        virtual CpuStatus step_cpu() = 0;
        virtual bool start(uintptr_t address) = 0;
        virtual void stop() = 0;

        // hooking
        virtual bool add_code_hook(uint64_t begin, uint64_t end, CodeHookFn callback, void* user_data) = 0;

        std::vector<TrapEntry> trap_table; // index = (TRAP_BASE + (0x10 * trap table index)). ik that this shouldn't be here, it should be in emulator

    private:
        CpuStatus status;
};
