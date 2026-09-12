#include "../pe/PEManager.hpp"
#include "../memory/memory.hpp"
#include "../debug/Debug.hpp"

class Emulator {
    public: 
        Emulator(CPU* cpu);
        bool load_driver(PE* pe);
        
        void handle_hook_trap();
        
    private:
        Stack stack;
        Heap heap;
        CPU* cpu;
        PEManager pe_manager;
        Debug debug;
};