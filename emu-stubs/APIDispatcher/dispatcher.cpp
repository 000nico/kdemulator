#include "APIDispatcher.hpp"
#include "../emu-core/src/memory/layout.hpp"
#include "../api/out/DbgPrint.hpp"
#include "../api/out/KdPrint.hpp"
#include "../api/out/DbgPrintEx.hpp"

APIDispatcher::APIDispatcher(){
    register_api("DbgPrint", new ApiDbgPrint());
    register_api("KdPrint",  new ApiKdPrint());
    register_api("DbgPrintEx", new ApiDbgPrintEx());
}

void APIDispatcher::register_api(const std::string &name, Api *api){
    apis[name] = api;
}

void emulate_ret(CPU* cpu, uint64_t val){
    // simulate ret (return), which is stored in rax
    cpu->set_register(REG_RAX, val);

    // pop return addres from stack and jump
    uint64_t rsp = cpu->get_register(REG_RSP);
    uint64_t ret_addr = 0;
    cpu->mem_read(rsp, &ret_addr, sizeof(uint64_t));
    cpu->set_register(REG_RSP, rsp + 8);
    cpu->set_register(REG_RIP, ret_addr);
}

void APIDispatcher::invoke(const std::string& name, CPU* cpu) {
    auto it = apis.find(name);
    uint64_t ret_val = 0;
    if(it != apis.end()) {
        ret_val = it->second->call(cpu);
    }

    else{
        Debug::debug_msg("Stub " + name + " not implemented, returning 0", LOG_WARN);
    }

    emulate_ret(cpu, ret_val);
}

bool APIDispatcher::resolve(CPU *cpu){
    uint64_t index = (cpu->get_register(REG_RIP) - HOOK_TRAP_BASE) / 0x10; // hooks are mapped with (hook base + (0x10 * i))
    
    TrapEntry call = cpu->trap_table.at(index);
    
    Debug::debug_msg("API_CALL[fn= " + call.function_name + ", module= " + call.module_name + "]", LOG_WARN);

    invoke(call.function_name, cpu);
    return true;
}