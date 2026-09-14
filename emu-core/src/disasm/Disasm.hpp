#pragma once
#include <Zydis/Zydis.h>
#include <cstdint>
#include <string>

class Disasm {
    public:
        Disasm();
        void init();
        void decompileRawBytes(const uint8_t* raw, size_t size, uintptr_t address);
        bool decompileSingleInstruction(const uint8_t* raw, size_t size, uintptr_t address, std::string* out_text = nullptr);
        
    private:
        ZydisDecoder decoder;
        ZydisFormatter formatter;
        bool initialized = false;
};

