#pragma once
#include "../Api.hpp"
#include "../../../emu-core/src/debug/Debug.hpp"
#include <string>
#include "UNICODE_STRING.hpp"

class ApiRtlInitUnicodeString : public Api {
    public:
        uint64_t call(CPU* cpu) override {
            uint64_t destPtr = get_arg(cpu, 0);
            uint64_t srcPtr = get_arg(cpu, 1);

            UNICODE_STRING dest = {0};
            std::string logText = "";

            if (srcPtr != 0) {
                std::wstring wideStr = L"";
                uint64_t curr = srcPtr;
                wchar_t ch = 0;

                while (true) {
                    cpu->mem_read(curr, &ch, sizeof(wchar_t));
                    if (ch == L'\0') break;
                    wideStr += ch;
                    curr += sizeof(wchar_t);
                }

                USHORT byteLen = static_cast<USHORT>(wideStr.length() * sizeof(wchar_t));
                dest.Length = byteLen;
                dest.MaximumLength = byteLen + sizeof(wchar_t); 
                dest.Buffer = reinterpret_cast<PWSTR>(srcPtr);

                logText = std::string(wideStr.begin(), wideStr.end());
            }

            cpu->mem_write(destPtr, &dest, sizeof(UNICODE_STRING));

            Debug::debug_msg("[RtlInitUnicodeString] text = \"" + logText + "\"", LOG_INFO);
            return 0;
        }
};