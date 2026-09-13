#include "Emulator.hpp"
#include "../cpu/perms.hpp"
#include "../memory/layout.hpp"
#include <cstdint>
#include <sstream>

static std::string hex64(uint64_t v) {
    std::ostringstream oss;
    oss << "0x" << std::uppercase << std::hex << v;
    return oss.str();
}

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

void trapHookCallback(CPU* cpu, uint64_t address, void* user_data){
    Debug::debug_msg("API Called trapped", LOG_WARN);
}

void codeHookCallback(CPU* cpu, uint64_t address, void* user_data){
    Debug::debug_msg("Executing at: " + hex64(address), LOG_INFO);
}

bool Emulator::start(PE* pe){
    if(!pe) {
        Debug::debug_msg("start: PE pointer is null, no driver loaded\n", LOG_ERROR);
        return false;
    }

    cpu->mem_map(STACK_BASE, STACK_SIZE, PROT_READ | PROT_WRITE);
    cpu->mem_map(HEAP_BASE, HEAP_SIZE, PROT_READ | PROT_WRITE);
    Debug::debug_msg("Stack and heap mapped", LOG_INFO);
    
    cpu->set_register(REG_RSP, STACK_BASE + STACK_SIZE - 0x1000);
    cpu->set_register(REG_RBP, STACK_BASE + STACK_SIZE - 0x1000);
    Debug::debug_msg("RBP and RSP set", LOG_INFO);

    cpu->mem_map(HOOK_TRAP_ADDR, 0x1000, PROT_ALL);
    cpu->add_code_hook(HOOK_TRAP_ADDR, HOOK_TRAP_ADDR + 0xFFF, trapHookCallback, NULL);

    //cpu->add_code_hook(CODE_BASE, CODE_BASE + pe->image_optional_header.SizeOfImage, codeHookCallback, NULL);
    
    uint64_t entry_point = CODE_BASE + pe->image_optional_header.AddressOfEntryPoint;
    cpu->set_register(REG_RIP, entry_point);
    return cpu->start(entry_point);
}
