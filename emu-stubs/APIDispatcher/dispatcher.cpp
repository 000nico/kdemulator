#include "APIDispatcher.hpp"
#include "../api/ob/IoCreateDevice.hpp"
#include "../emu-core/src/memory/layout.hpp"
#include "../api/out/DbgPrint.hpp"
#include "../api/out/KdPrint.hpp"
#include "../api/out/DbgPrintEx.hpp"
#include "../api/memory/ExAllocatePool.hpp"
#include "../api/memory/ExAllocatePool2.hpp"
#include "../api/memory/ExAllocatePool3.hpp"
#include "../api/memory/ExAllocatePoolWithTag.hpp"
#include "../api/memory/ExFreePoolWithTag.hpp"
#include "../api/memory/ExFreePool.hpp"
#include "../api/rtl/RtlInitUnicodeString.hpp"
#include "../api/rtl/RtlZeroMemory.hpp"
#include "../api/rtl/RtlCopyMemory.hpp"
#include "../api/rtl/RtlCopyUnicodeString.hpp"
#include "../api/rtl/RtlUnicodeStringToInteger.hpp"
#include "../api/rtl/RtlIntegerToUnicodeString.hpp"
#include "../api/rtl/RtlAppendUnicodeStringToString.hpp"

APIDispatcher::APIDispatcher(){
    register_api("DbgPrint", new ApiDbgPrint());
    register_api("KdPrint",  new ApiKdPrint());
    register_api("DbgPrintEx", new ApiDbgPrintEx());
    register_api("ExAllocatePool", new ApiExAllocatePool());
    register_api("ExAllocatePool2", new ApiExAllocatePool2());
    register_api("ExAllocatePool3", new ApiExAllocatePool3());
    register_api("ExAllocatePoolWithTag", new ApiExAllocatePoolWithTag());
    register_api("ExFreePool", new ApiExFreePool());
    register_api("ExFreePoolWithTag", new ApiExFreePoolWithTag());
    register_api("RtlZeroMemory", new ApiRtlZeroMemory());
    register_api("RtlCopyMemory", new ApiRtlCopyMemory());
    register_api("RtlCopyUnicodeString", new ApiRtlCopyUnicodeString());
    register_api("RtlInitUnicodeString", new ApiRtlInitUnicodeString());
    register_api("RtlUnicodeStringToInteger", new ApiRtlUnicodeStringToInteger());
    register_api("RtlIntegerToUnicodeString", new ApiRtlIntegerToUnicodeString());
    register_api("RtlAppendUnicodeStringToString", new ApiRtlAppendUnicodeStringToString());
    register_api("IoCreateDevice", new ApiIoCreateDevice());
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