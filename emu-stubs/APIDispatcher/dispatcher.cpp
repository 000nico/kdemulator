#include "APIDispatcher.hpp"
#include "../emu-core/src/memory/layout.hpp"

bool APIDispatcher::resolve(CPU *cpu){
    uint64_t index = (cpu->get_register(REG_RIP) - HOOK_TRAP_BASE); // hooks are mapped with (hook base + (0x10 * i))
    TrapEntry call = cpu->trap_table[index];
    
    Debug::debug_msg("API_CALL[fn= " + call.function_name + ", module= " + call.module_name + "]", LOG_WARN);


    // hacer el switch y si no se resuelve printear que se llamo
    return true;
}