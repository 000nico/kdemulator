#pragma once
#include "../Api.hpp"
#include "../../../emu-core/src/debug/Debug.hpp"
#include "UNICODE_STRING.hpp"
#include <string>
#include <vector>

class ApiRtlAppendUnicodeStringToString : public Api {
    public:
        uint64_t call(CPU* cpu) override {
            uint64_t destinationPtr = get_arg(cpu, 0); // PUNICODE_STRING Destination
            uint64_t sourcePtr = get_arg(cpu, 1);      // PCUNICODE_STRING Source

            UNICODE_STRING destStruct;
            UNICODE_STRING sourceStruct;

            cpu->mem_read(destinationPtr, &destStruct, sizeof(UNICODE_STRING));
            cpu->mem_read(sourcePtr, &sourceStruct, sizeof(UNICODE_STRING));
            
            if (sourceStruct.Length == 0 || sourceStruct.Buffer == 0) 
                return 0;
            
            USHORT newLength = destStruct.Length + sourceStruct.Length;
            if (newLength > destStruct.MaximumLength) {
                // STATUS_BUFFER_TOO_SMALL (0xC0000023 en NTSTATUS)
                Debug::debug_msg("[RtlAppendUnicodeStringToString] Error: Buffer too small!", LOG_WARN);
                return 0xC0000023; 
            }

            std::vector<uint8_t> sourceData(sourceStruct.Length);
            cpu->mem_read(reinterpret_cast<uint64_t>(sourceStruct.Buffer), sourceData.data(), sourceStruct.Length);

            uint64_t targetAddress = reinterpret_cast<uint64_t>(destStruct.Buffer) + destStruct.Length;
            cpu->mem_write(targetAddress, sourceData.data(), sourceStruct.Length);

            destStruct.Length = newLength;
            cpu->mem_write(destinationPtr, &destStruct, sizeof(UNICODE_STRING));

            Debug::debug_msg("[RtlAppendUnicodeStringToString] Appended bytes = " + std::to_string(sourceStruct.Length) + 
                             ", new total length = " + std::to_string(newLength), LOG_INFO);

            return 0; // STATUS_SUCCESS
        }
};