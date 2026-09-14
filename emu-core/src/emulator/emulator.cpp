#include "Emulator.hpp"
#include "../cpu/perms.hpp"
#include "../memory/layout.hpp"
#include "hooks.hpp"
#include <cstdint>
#include <hex.hpp>
#include <vector>

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

void map_stack_and_heap(CPU* cpu){
    cpu->mem_map(STACK_BASE, STACK_SIZE, PROT_READ | PROT_WRITE);
    cpu->mem_map(HEAP_BASE, HEAP_SIZE, PROT_READ | PROT_WRITE);
    Debug::debug_msg("Stack and heap mapped", LOG_INFO);
}

void set_rbp_rsp(CPU* cpu){
    cpu->set_register(REG_RSP, STACK_BASE + STACK_SIZE - 0x1000);
    cpu->set_register(REG_RBP, STACK_BASE + STACK_SIZE - 0x1000);
    Debug::debug_msg("RBP and RSP set", LOG_INFO);
}

void set_entry_point(CPU* cpu, PE* pe){
    uint64_t entry_point = CODE_BASE + pe->image_optional_header.AddressOfEntryPoint;
    cpu->set_register(REG_RIP, entry_point);
}

bool Emulator::start(PE* pe){
    if(!pe) {
        Debug::debug_msg("start: PE pointer is null, no driver loaded\n", LOG_ERROR);
        return false;
    }

    map_stack_and_heap(this->cpu);
    set_rbp_rsp(this->cpu);
    set_entry_point(this->cpu, pe);

    addCodeHookCallback(this->cpu, pe);
    addTrapHookCallback(this->cpu);
    
    return cpu->start(CODE_BASE + pe->image_optional_header.AddressOfEntryPoint);
}
