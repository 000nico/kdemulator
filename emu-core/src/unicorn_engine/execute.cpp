#include "UnicornEngine.hpp"
#include "../memory/layout.hpp"

CpuStatus UnicornEngine::step_cpu(){
    uintptr_t current_rip = this->get_register(REG_RIP);
    
    if((current_rip & 0xFFFFFFFF00000000) == HOOK_TRAP_ADDR)
        return CpuStatus::HOOK_TRAP;

    // manage single step
    
    return CpuStatus::OK;
}

bool UnicornEngine::start(uintptr_t address){
    this->set_register(REG_RIP, address);

    // setup stack
    this->set_register(REG_RSP, STACK_BASE - STACK_SIZE - 0x1000);
    return uc_emu_start(this->uc, address, 0, 0, 0) == UC_ERR_OK;
}