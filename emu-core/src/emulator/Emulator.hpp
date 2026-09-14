#pragma once
#include "../pe/PEManager.hpp"
#include "../memory/memory.hpp"
#include "../debug/Debug.hpp"
#include "../pe/TrapEntry.hpp"
#include <vector>

class Emulator {
    public: 
        Emulator(CPU* cpu);
        
        bool load_driver(PE* pe);
        bool start(PE* pe);
        
        void handle_hook_trap();
        
    private:
        Stack stack;
        Heap heap;
        CPU* cpu;
        PEManager pe_manager;
        Debug debug;
};