#pragma once
#include "../Api.hpp"
#include "../../../emu-core/src/debug/Debug.hpp"
#include <string>
#include "UNICODE_STRING.hpp"

class ApiRtlCopyUnicodeString : public Api {
    public:
        uint64_t call(CPU* cpu) override {
            uint64_t destination = get_arg(cpu, 0);
            uint64_t source = get_arg(cpu, 1);

            UNICODE_STRING bufferSrc;
            UNICODE_STRING bufferDest;
            
            /* If SourceString is NULL, this routine sets the Length field of the structure pointed to by DestinationString to zero. */
            if(source == NULL){
                USHORT zero = 0;
                cpu->mem_write(destination, &zero, sizeof(USHORT));
                return 0;
            }
            
            cpu->mem_read(source, &bufferSrc, sizeof(UNICODE_STRING));
            cpu->mem_read(destination, &bufferDest, sizeof(UNICODE_STRING));

            /* The number of bytes copied from the source string is either the source string length (specified by the Length member of the structure pointed to by SourceString) or the maximum length of the destination string (specified by the MaximumLength member of the structure pointed to by DestinationString), whichever is smaller. */
            size_t size = (bufferSrc.Length < bufferDest.MaximumLength) ? bufferSrc.Length : bufferDest.MaximumLength;

            if (size > 0 && bufferSrc.Buffer != 0 && bufferDest.Buffer != 0) {
                std::vector<uint8_t> stringData(size);
                cpu->mem_read(reinterpret_cast<uint64_t>(bufferSrc.Buffer), stringData.data(), size);
                cpu->mem_write(reinterpret_cast<uint64_t>(bufferDest.Buffer), stringData.data(), size);
            }
            
            USHORT newLength = static_cast<USHORT>(size);
            cpu->mem_write(destination, &newLength, sizeof(USHORT));
                        
                Debug::debug_msg("[RtlCopyUnicodeString] dest = " + std::to_string(destination) + 
                    ", source = " + std::to_string(source) + 
                    ", length = " + std::to_string(size), LOG_INFO);
                
            return 0;
        }
};