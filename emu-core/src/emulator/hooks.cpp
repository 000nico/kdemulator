#include "hooks.hpp"
#include "../debug/Debug.hpp"
#include "../cpu/CPU.hpp"
#include "../memory/layout.hpp"
#include "../cpu/perms.hpp"
#include "../emu-stubs/APIDispatcher/APIDispatcher.hpp"
#include <hex.hpp>

// this function calls the resolver of API's
void trapHookCallback(CPU* cpu, uint64_t address, void* user_data){
    APIDispatcher::resolve(cpu);
}

void codeHookCallback(CPU* cpu, uint64_t address, void* user_data){
    Debug::debug_msg("Executing at: " + hex64(address), LOG_INFO);
}

void addTrapHookCallback(CPU* cpu){
    cpu->mem_map(HOOK_TRAP_BASE, HOOK_TRAP_SIZE, PROT_ALL);
    cpu->add_code_hook(HOOK_TRAP_BASE, HOOK_TRAP_SIZE, trapHookCallback, NULL);
}

void addCodeHookCallback(CPU* cpu, PE* pe){
    cpu->add_code_hook(CODE_BASE, CODE_BASE + pe->image_optional_header.SizeOfImage, codeHookCallback, NULL);
}