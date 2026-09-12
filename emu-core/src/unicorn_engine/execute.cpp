#include "UnicornEngine.hpp"
#include "../memory/layout.hpp"

CpuStatus UnicornEngine::step_cpu(){
    uintptr_t current_rip = this->get_register(REG_RIP);
    
    if((current_rip & 0xFFFFFFFF00000000) == HOOK_TRAP_ADDR)
        return CpuStatus::HOOK_TRAP;

    // manage single step
    
    return CpuStatus::OK;
}