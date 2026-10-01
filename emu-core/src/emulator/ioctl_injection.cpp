#include "../pe/format.hpp"
#include "../cpu/CPU.hpp"
#include "../cpu/perms.hpp"
#include "../debug/Debug.hpp"
#include "../memory/layout.hpp"
#include "../disasm/Disasm.hpp" 
#include "../sdk/hex.hpp"
#include "ioctl_injection.hpp"
#include "../kernel/include/structs.hpp"

constexpr uint64_t DISPATCH_RETURN_SENTINEL = 0xFFFFDEAD00000001ULL;
static bool g_dispatch_sentinel_mapped = false;

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

static uint64_t g_dispatch_table[27] = {0};

void dump_dispatch_table(CPU* cpu) {
    DRIVER_OBJECT driver_object = {};
    if(!cpu->mem_read(STRUCT_BASE, &driver_object, sizeof(DRIVER_OBJECT))){
        Debug::debug_msg("could not read driver object", LOG_ERROR);
        return;
    }

    for(int i = 0; i < 27; i++) {
        g_dispatch_table[i] = (uint64_t)driver_object.MajorFunction[i]; 

        if(driver_object.MajorFunction[i] == 0)
            continue;

        Debug::debug_msg("Major function [" + std::to_string(i) + "]" + " = " + IrpMjNames[i]  + " = " + hex64((uint64_t)driver_object.MajorFunction[i]), LOG_INFO);
    }
}

uint64_t get_major_function(int irp_mj_index) {
    return g_dispatch_table[irp_mj_index];
}

uint64_t build_irp(CPU* cpu, uint32_t ioctl_code, void* input_data, size_t input_len, size_t output_len) {
    // io_stack_location HEAP_BASE
    IO_STACK_LOCATION isl = {};
    isl.MajorFunction = 0x0E; // IRP_MJ_DEVICE_CONTROL
    isl.IoControlCode = ioctl_code;
    isl.OutputBufferLength = static_cast<uint32_t>(output_len);
    isl.InputBufferLength = static_cast<uint32_t>(input_len);
    if(!cpu->mem_write(HEAP_BASE, &isl, sizeof(IO_STACK_LOCATION)))
        Debug::debug_msg("build_irp: failed writing IO_STACK_LOCATION", LOG_ERROR);

    if (input_data != nullptr && input_len > 0) {
        if(!cpu->mem_write(HEAP_BASE + 0x450, input_data, input_len))
            Debug::debug_msg("build_irp: failed writing input buffer", LOG_ERROR);
    }

    IRP irp = {};
    irp.Tail_Overlay_CurrentStackLocation = HEAP_BASE;          
    irp.AssociatedIrp_SystemBuffer        = HEAP_BASE + 0x450;  
    if(!cpu->mem_write(HEAP_BASE + 0x48, &irp, sizeof(IRP)))
        Debug::debug_msg("build_irp: failed writing IRP", LOG_ERROR);

    return HEAP_BASE + 0x48; 
}

void dispatch_ret_callback(CPU* cpu, uint64_t address, void* user_data) {
    uint64_t rax = cpu->get_register(REG_RAX);
    Debug::debug_msg("Dispatch routine returned, NTSTATUS = " + hex64((uint64_t)(uint32_t)rax), LOG_INFO);
    cpu->stop();
}

void invoke_major_function(CPU* cpu, uint32_t ioctl_code, void* input_data, size_t input_len, size_t output_len) {
    uint64_t target = get_major_function(0x0E);
    if (target == 0) {
        Debug::debug_msg("MajorFunction[DEVICE_CONTROL] is null, nothing to call", LOG_ERROR);
        return;
    }

    uint64_t irp_addr = build_irp(cpu, ioctl_code, input_data, input_len, output_len);

    if (!g_dispatch_sentinel_mapped) {
        uint64_t page = DISPATCH_RETURN_SENTINEL & ~0xFFFULL;
        cpu->mem_map(page, 0x1000, PROT_READ | PROT_EXEC);

        uint8_t hlt = 0xF4;
        cpu->mem_write(DISPATCH_RETURN_SENTINEL, &hlt, 1);

        cpu->add_code_hook(DISPATCH_RETURN_SENTINEL, DISPATCH_RETURN_SENTINEL, dispatch_ret_callback, nullptr);
        g_dispatch_sentinel_mapped = true;
    }

    uint64_t rsp = cpu->get_register(REG_RSP);
    rsp -= 8;

    uint64_t sentinel_value = DISPATCH_RETURN_SENTINEL;
    cpu->mem_write(rsp, &sentinel_value, sizeof(sentinel_value));
    cpu->set_register(REG_RSP, rsp);

    cpu->set_register(REG_RCX, STRUCT_BASE + OFF_DEVOBJ);
    cpu->set_register(REG_RDX, irp_addr);

    cpu->start(target);
}