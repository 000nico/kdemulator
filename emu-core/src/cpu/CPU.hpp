#include "registers.hpp"
#include <cstdint>

class CPU {
    public:
        virtual ~CPU() {}

        // registers
        virtual bool set_register(Register reg, uint64_t value) = 0;
        virtual int get_register(Register reg) = 0;

        // stack
        virtual uintptr_t get_stack_base() = 0;
        virtual size_t get_stack_size() = 0;

        // heap
        virtual uintptr_t get_heap_base() = 0;
        virtual size_t get_heap_size() = 0;

        // memory
        virtual bool mem_map(uintptr_t address, size_t size) = 0;
        virtual bool mem_write(uintptr_t address, void* data, size_t size) = 0;
        virtual bool mem_read(uintptr_t address, void* buffer, size_t size) = 0;

        virtual bool start() = 0;
};
