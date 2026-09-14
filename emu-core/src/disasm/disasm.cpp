#include "Disasm.hpp"
#include "../debug/Debug.hpp"
#include <hex.hpp>

Disasm::Disasm() {
    init();
}

void Disasm::init(){
    if (initialized) return;
    ZydisDecoderInit(&decoder, ZYDIS_MACHINE_MODE_LONG_64, ZYDIS_STACK_WIDTH_64);
    ZydisFormatterInit(&formatter, ZYDIS_FORMATTER_STYLE_INTEL);
    initialized = true;
}

bool Disasm::decompileSingleInstruction(const uint8_t* raw, size_t size, uintptr_t address, std::string* out_text){
    if (!initialized) init();

    ZydisDecodedInstruction instruction;
    ZydisDecodedOperand operands[ZYDIS_MAX_OPERAND_COUNT];

    if (ZYAN_SUCCESS(ZydisDecoderDecodeFull(&decoder, raw, size, &instruction, operands))) {
        char buffer[256];
        ZydisFormatterFormatInstruction(&formatter, &instruction, operands, instruction.operand_count_visible, buffer, sizeof(buffer), address, NULL);
        
        std::string disasm_str = buffer;
        if (out_text) {
            *out_text = disasm_str;
        }

        Debug::add_trace(address, disasm_str);
        if (Debug::traceMode) {
            Debug::debug_msg(hex64(address) + ": " + disasm_str + "\n", LOG_INFO);
        }
        return true;
    }
    return false;
}

void Disasm::decompileRawBytes(const uint8_t* raw, size_t size, uintptr_t address){
    if (!initialized) init();

    ZydisDecodedInstruction instruction;
    ZydisDecodedOperand operands[ZYDIS_MAX_OPERAND_COUNT];

    ZyanU64 runtime_address = address;
    const uint8_t *read_ptr = raw;
    ZyanUSize length_left = size;

    while (length_left > 0 && ZYAN_SUCCESS(ZydisDecoderDecodeFull(&decoder, read_ptr, length_left, &instruction, operands))) {
        char buffer[256];
        ZydisFormatterFormatInstruction(&formatter, &instruction, operands, instruction.operand_count_visible, buffer, sizeof(buffer), runtime_address, NULL);
    
        std::string disasm_str = buffer;
        Debug::add_trace(runtime_address, disasm_str);
        if (Debug::traceMode) {
            Debug::debug_msg(hex64(runtime_address) + ": " + disasm_str + "\n", LOG_INFO);
        }
    
        read_ptr += instruction.length;
        length_left -= instruction.length;
        runtime_address += instruction.length;
    }
}

