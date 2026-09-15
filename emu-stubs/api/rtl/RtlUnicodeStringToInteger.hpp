#pragma once
#include "../Api.hpp"
#include "../../../emu-core/src/debug/Debug.hpp"
#include "UNICODE_STRING.hpp"
#include <string>
#include <vector>

class ApiRtlUnicodeStringToInteger : public Api {
    public:
        uint64_t call(CPU* cpu) override {
            uint64_t stringPtr = get_arg(cpu, 0);   // PUNICODE_STRING
            uint64_t base = get_arg(cpu, 1);        // ULONG
            uint64_t valuePtr = get_arg(cpu, 2);    // PULONG / PULONG64

            UNICODE_STRING uString;
            cpu->mem_read(stringPtr, &uString, sizeof(UNICODE_STRING));

            std::wstring wideStr = L"";
            if (uString.Buffer != 0 && uString.Length > 0) {
                size_t charCount = uString.Length / sizeof(wchar_t);
                std::vector<wchar_t> buffer(charCount + 1, 0);
                
                cpu->mem_read(reinterpret_cast<uint64_t>(uString.Buffer), buffer.data(), uString.Length);
                wideStr = buffer.data();
            }

            int actualBase = static_cast<int>(base);
            if (actualBase == 0) 
                actualBase = wideStr.length() > 2 && wideStr[0] == L'0' && (wideStr[1] == L'x' || wideStr[1] == L'X')? 16 : 10;

            uint64_t resultValue = 0;
            try {
                std::string narrowStr(wideStr.begin(), wideStr.end());
                resultValue = std::stoull(narrowStr, nullptr, actualBase);
            } catch (...) {
                resultValue = 0; 
            }

            cpu->mem_write(valuePtr, &resultValue, sizeof(uint32_t)); 

            Debug::debug_msg("[RtlUnicodeStringToInteger] base = " + std::to_string(actualBase) + 
                             ", result = " + std::to_string(resultValue), LOG_INFO);

            return 0; // STATUS_SUCCESS
        }
};