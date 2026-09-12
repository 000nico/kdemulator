#include "Emulator.hpp"
#include "../memory/layout.hpp"

bool Emulator::load_driver(PE* pe){
    if(!this->pe_manager.load_pe(this->cpu, *pe, CODE_BASE)) return false;

    return true;
}

bool Emulator::start(PE* pe){
    uint64_t entry_point = CODE_BASE + pe->image_optional_header.AddressOfEntryPoint;
    cpu->set_register(REG_RIP, entry_point);
    cpu->start(entry_point);
}