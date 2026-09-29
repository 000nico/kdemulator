#include "../pe/format.hpp"
#include "../cpu/CPU.hpp"
#include "../debug/Debug.hpp"
#include "../memory/layout.hpp"
#include "../disasm/Disasm.hpp" 
#include "../sdk/hex.hpp"
#include "ioctl_injection.hpp"
#include "../kernel/include/structs.hpp"

void dump_dispatch_table(CPU* cpu);

uint64_t get_driver_entry_ret_absolute_address(PE* pe, CPU* cpu, Disasm& disasm) {
    uint64_t entry_point = CODE_BASE + pe->image_optional_header.AddressOfEntryPoint;
    
    std::vector<uint8_t> buffer(4096);
    if (!cpu->mem_read(entry_point, buffer.data(), buffer.size())) {
        Debug::debug_msg("Failed to read memory at entry point for disassembling", LOG_ERROR);
        return 0;
    }

    uint64_t runtime_address = entry_point;
    const uint8_t* read_ptr = buffer.data();
    size_t length_left = buffer.size();

    ZydisDecodedInstruction instruction;
    ZydisDecodedOperand operands[ZYDIS_MAX_OPERAND_COUNT];

    while (length_left > 0 && ZYAN_SUCCESS(ZydisDecoderDecodeFull(&disasm.decoder, read_ptr, length_left, &instruction, operands))) {
        
        if (instruction.mnemonic == ZYDIS_MNEMONIC_RET) {
            Debug::debug_msg("found exact RET of entry point: " + hex64(runtime_address), LOG_INFO);
            return runtime_address;
        }

        read_ptr += instruction.length;
        length_left -= instruction.length;
        runtime_address += instruction.length;
    }

    Debug::debug_msg("did not find RET of entry point", LOG_ERROR);
    return 0;
}

void driver_entry_ret_callback(CPU* cpu, uint64_t address, void* user_data) {
    uint64_t rax = cpu->get_register(REG_RAX);

    int32_t status = (int32_t)rax;
    Debug::debug_msg("NTSTATUS = " + hex64((uint64_t)(uint32_t)status), LOG_INFO);

    if (status >= 0) {
        Debug::debug_msg("DriverEntry (NT_SUCCESS)", LOG_INFO);
    } else {
        Debug::debug_msg("DriverEntry failed", LOG_ERROR);
    }

    dump_dispatch_table(cpu);
}

void add_hook_on_driver_entry_ret(CPU* cpu, uint64_t address) {
    cpu->add_code_hook(address, address, driver_entry_ret_callback, 0);
}

void dump_dispatch_table(CPU* cpu) {
    DRIVER_OBJECT driver_object = {};
    //Debug::debug_msg("sizeof(DRIVER_OBJECT) = " + std::to_string(sizeof(DRIVER_OBJECT)), LOG_INFO);
    if(!cpu->mem_read(STRUCT_BASE, &driver_object, sizeof(DRIVER_OBJECT))){
        Debug::debug_msg("could not read driver object", LOG_ERROR);
    }

    for(int i = 0; i < 27; i++) {
        if(driver_object.MajorFunction[i] == 0)
            continue;

        else {
            Debug::debug_msg("Major function [" + std::to_string(i) + "]" + " = " + IrpMjNames[i]  + " = " + hex64((uint64_t)driver_object.MajorFunction[i]), LOG_INFO);
        }
    }
}