#include "Emulator.hpp"
#include "../memory/layout.hpp"

bool Emulator::load_driver(PE* pe){
    // load pe
    if(!this->pe_manager.load_pe(this->cpu, pe)) return false;

    // configure stack
    uint64_t entry_point = CODE_BASE + pe->image_optional_header.AddressOfEntryPoint;
    cpu->set_register(REG_RIP, entry_point);
    
    return true;
}