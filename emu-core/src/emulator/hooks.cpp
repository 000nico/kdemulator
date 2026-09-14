#include "hooks.hpp"
#include "../debug/Debug.hpp"
#include "../cpu/CPU.hpp"
#include "../memory/layout.hpp"
#include "../cpu/perms.hpp"
#include "../emu-stubs/APIDispatcher/APIDispatcher.hpp"
#include <hex.hpp>

// this function calls the resolver of API's
void trapHookCallback(CPU* cpu, uint64_t address, void* user_data){
    APIDispatcher* dispatcher = static_cast<APIDispatcher*>(user_data);
    dispatcher->resolve(cpu);
}

void codeHookCallback(CPU* cpu, uint64_t address, void* user_data){
    Disasm* disasm = static_cast<Disasm*>(user_data);
    static Disasm fallback_disasm;
    Disasm* active_disasm = disasm ? disasm : &fallback_disasm;

    uint8_t raw_bytes[15] = {0};
    if (cpu->mem_read(address, raw_bytes, sizeof(raw_bytes))) {
        active_disasm->decompileSingleInstruction(raw_bytes, sizeof(raw_bytes), address);
    } else {
        if (Debug::traceMode) {
            Debug::debug_msg("Executing at: " + hex64(address) + " (failed to read memory)\n", LOG_WARN);
        }
    }
}


void addTrapHookCallback(CPU* cpu, APIDispatcher* dispatcher){
    cpu->mem_map(HOOK_TRAP_BASE, HOOK_TRAP_SIZE, PROT_ALL);
    cpu->add_code_hook(HOOK_TRAP_BASE, HOOK_TRAP_BASE + HOOK_TRAP_SIZE, trapHookCallback, dispatcher);
}

void addCodeHookCallback(CPU* cpu, PE* pe, Disasm* disasm){
    cpu->add_code_hook(CODE_BASE, CODE_BASE + pe->image_optional_header.SizeOfImage, codeHookCallback, disasm);
}