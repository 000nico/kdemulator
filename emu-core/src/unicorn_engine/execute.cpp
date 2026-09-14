#include "UnicornEngine.hpp"
#include "../memory/layout.hpp"
#include "../debug/Debug.hpp"
#include <hex.hpp>

CpuStatus UnicornEngine::step_cpu(){
    uintptr_t current_rip = this->get_register(REG_RIP);
    
    if((current_rip & 0xFFFFFFFF00000000) == HOOK_TRAP_ADDR) {
        Debug::debug_msg("execute: hook trap detected at RIP=" + hex64(current_rip) + "\n", LOG_WARN);
        return CpuStatus::HOOK_TRAP;
    }

    // manage single step
    
    return CpuStatus::OK;
}

bool UnicornEngine::start(uintptr_t address){
    this->set_register(REG_RIP, address);

    Debug::debug_msg("execute: starting emulation at " + hex64(address) + "\n", LOG_INFO);

    uc_err err = uc_emu_start(this->uc, address, 0, 0, 0);
    if (err != UC_ERR_OK) {
        Debug::debug_msg("execute: uc_emu_start failed at " + hex64(get_register(REG_RIP)) + " err=" + uc_strerror(err) + "\n", LOG_ERROR);
        return false;
    }

    Debug::debug_msg("execute: emulation finished OK\n", LOG_INFO);
    return true;
}
