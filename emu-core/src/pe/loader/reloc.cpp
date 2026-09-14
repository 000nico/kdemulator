#include "loader.hpp"
#include "../../debug/Debug.hpp"

bool resolve_reloc(CPU* cpu, PE pe, uintptr_t address){
    uintptr_t relocRVA  = pe.image_optional_header.BaseRelocationTableVA;
    uintptr_t relocSize = pe.image_optional_header.BaseRelocationTableSize;

    if (relocRVA == 0 || relocSize == 0) return true;

    uintptr_t currentBlockAddr = address + relocRVA;
    uintptr_t endRelocAddr = currentBlockAddr + relocSize;
    
    while(currentBlockAddr < endRelocAddr) {
        IMAGE_BASE_RELOCATION blockHeader = {0};
        if(!cpu->mem_read(currentBlockAddr, &blockHeader, sizeof(IMAGE_BASE_RELOCATION))) {
            Debug::debug_msg("resolve_reloc: mem_read failed reading block header at " + hex64(currentBlockAddr) + "\n", LOG_ERROR);
            return false;
        }

        if(blockHeader.SizeOfBlock == 0) break; // reached the end

        // immediatly after the header there is the block data
        size_t entry_amount = (blockHeader.SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION)) / sizeof(uint16_t);
        uintptr_t entriesStartAddr = currentBlockAddr + sizeof(IMAGE_BASE_RELOCATION);
        
        for(size_t i = 0; i < entry_amount; i++){
            // every entry is a 16 bits // WORD
            uint16_t entry;
            if(!cpu->mem_read(entriesStartAddr + (i * sizeof(uint16_t)), &entry, sizeof(uint16_t))) {
                Debug::debug_msg("resolve_reloc: mem_read failed reading entry " + std::to_string(i) + " at " + hex64(entriesStartAddr) + "\n", LOG_ERROR);
                return false;
            }
    
            // first 4 bits
            uint8_t type = (entry >> 12);
    
            // 12 last bits
            uint16_t offset = (entry & 0x0FFF);

            uintptr_t addr_to_patch = address + blockHeader.VirtualAddress + offset;

            if(type == 0) continue; // do nothing XD. 0 is IMAGE_REL_BASED_ABSOLUTE and gets ignored
                
            // 32 bit relocation, modify an unsigned int value of 32 bits
            else if (type == 3) {
                
                uint32_t new_value = 0;
                if(!cpu->mem_read(addr_to_patch, &new_value, sizeof(uint32_t))) {
                    Debug::debug_msg("resolve_reloc: mem_read failed reading 32-bit value at " + hex64(addr_to_patch) + "\n", LOG_ERROR);
                    return false;
                }
    
                new_value += address - pe.image_optional_header.ImageBase;
                if(!cpu->mem_write(addr_to_patch, &new_value, sizeof(uint32_t))) {
                    Debug::debug_msg("resolve_reloc: mem_write failed writing 32-bit reloc at " + hex64(addr_to_patch) + "\n", LOG_ERROR);
                    return false;
                }
            }
    
            // 64 bit relocation. modify an unsigned int value of 64 bits
            else if (type == 10) {
                uint64_t new_value = 0;
                if(!cpu->mem_read(addr_to_patch, &new_value, sizeof(uint64_t))) {
                    Debug::debug_msg("resolve_reloc: mem_read failed reading 64-bit value at " + hex64(addr_to_patch) + "\n", LOG_ERROR);
                    return false;
                }
    
                new_value += address - pe.image_optional_header.ImageBase;
                if(!cpu->mem_write(addr_to_patch, &new_value, sizeof(uint64_t))) {
                    Debug::debug_msg("resolve_reloc: mem_write failed writing 64-bit reloc at " + hex64(addr_to_patch) + "\n", LOG_ERROR);
                    return false;
                }
            }
        }
        // next block
        currentBlockAddr += blockHeader.SizeOfBlock;
    }
    
    return true;
}
