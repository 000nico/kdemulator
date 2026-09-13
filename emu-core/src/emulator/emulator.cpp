#include "Emulator.hpp"
#include "../memory/layout.hpp"

Emulator::Emulator(CPU* cpu) : cpu(cpu) {
    stack.base = STACK_BASE;
    stack.size = STACK_SIZE;
    heap.base = HEAP_BASE;
    heap.size = HEAP_SIZE;
}

bool Emulator::load_driver(PE* pe){
    if(!pe) {
        Debug::debug_msg("load_driver: PE pointer is null\n", LOG_ERROR);
        return false;
    }
    if(!this->pe_manager.load_pe(this->cpu, *pe, CODE_BASE)) return false;

    return true;
}

bool Emulator::start(PE* pe){
    if(!pe) {
        Debug::debug_msg("start: PE pointer is null, no driver loaded\n", LOG_ERROR);
        return false;
    }
    uint64_t entry_point = CODE_BASE + pe->image_optional_header.AddressOfEntryPoint;
    cpu->set_register(REG_RIP, entry_point);
    return cpu->start(entry_point);
}