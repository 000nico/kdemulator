#include "../memory/memory.hpp"
#include "../cpu/CPU.hpp"

class Emulator {
    public: 
        Emulator(CPU* cpu);
        
    private:
        Stack stack;
        Heap heap;
        CPU* cpu;
};