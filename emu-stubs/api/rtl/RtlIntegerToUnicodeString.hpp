#pragma once
#include "../Api.hpp"
#include "../../../emu-core/src/debug/Debug.hpp"
#include "UNICODE_STRING.hpp"
#include <string>
#include <cwchar>

class ApiRtlIntegerToUnicodeString : public Api {
    public:
        uint64_t call(CPU* cpu) override {
            uint64_t value = get_arg(cpu, 0);      // ULONG
            uint64_t base = get_arg(cpu, 1);        // ULONG
            uint64_t stringPtr = get_arg(cpu, 2);   // PUNICODE_STRING

            UNICODE_STRING destStruct;
            cpu->mem_read(stringPtr, &destStruct, sizeof(UNICODE_STRING));

            wchar_t buffer[64] = {0};
            int actualBase = (base == 0) ? 10 : static_cast<int>(base);

            if (actualBase == 16) {
                swprintf(buffer, sizeof(buffer) / sizeof(wchar_t), L"%llX", value);
            } else if (actualBase == 8) {
                swprintf(buffer, sizeof(buffer) / sizeof(wchar_t), L"%llo", value);
            } else {
                swprintf(buffer, sizeof(buffer) / sizeof(wchar_t), L"%llu", value);
            }

            std::wstring outString(buffer);
            USHORT byteLen = static_cast<USHORT>(outString.length() * sizeof(wchar_t));

            if (destStruct.Buffer != 0 && destStruct.MaximumLength >= byteLen + sizeof(wchar_t)) {
                cpu->mem_write(reinterpret_cast<uint64_t>(destStruct.Buffer), buffer, byteLen + sizeof(wchar_t));
                
                destStruct.Length = byteLen;
                cpu->mem_write(stringPtr, &destStruct, sizeof(UNICODE_STRING));
            }

            std::string logStr(outString.begin(), outString.end());
            Debug::debug_msg("[RtlIntegerToUnicodeString] value = " + std::to_string(value) + 
                             ", base = " + std::to_string(actualBase) + 
                             ", res = " + logStr, LOG_INFO);

            return 0; // STATUS_SUCCESS
        }
};