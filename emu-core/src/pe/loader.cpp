#include "PEManager.hpp"
#include "../memory/layout.hpp"
#include <cstdint>
#include <span>

bool load_headers(CPU* cpu, PE pe, uintptr_t address){
    if(!cpu->mem_map(address, pe.image_optional_header.SizeOfImage)) return false;

    // write headers
    uintptr_t map_in_addr = address;
    if(!cpu->mem_write(map_in_addr, &pe.image_dos_header, sizeof(pe.image_dos_header))) return false;

    map_in_addr += pe.image_dos_header.e_lfanew;
    
    if(!cpu->mem_write(map_in_addr, &pe.image_file_header, sizeof(pe.image_file_header))) return false;
    map_in_addr += sizeof(pe.image_file_header);
    
    if(!cpu->mem_write(map_in_addr, &pe.image_optional_header, sizeof(pe.image_optional_header))) return false;
    map_in_addr += sizeof(pe.image_optional_header);
    
    for(int i = 0; i < pe.sections.size(); i++){
        if(!cpu->mem_write(map_in_addr, &pe.sections[i], sizeof(pe.sections[i]))) return false;
        map_in_addr += sizeof(IMAGE_SECTION_HEADER);
    }
    
    return true;
}


bool load_sections(CPU* cpu, PE pe, uintptr_t address) {
    for (size_t i = 0; i < pe.sections.size(); i++) {
        if (pe.sections[i].SizeOfRawData == 0) continue; // if section has no data, do not map it

        uintptr_t section_dest = address + pe.sections[i].VirtualAddress;

        // slice of the raw section data, taken from raw data with pointer to raw data and size of raw data
        std::span<const uint8_t> section_slice(
            pe.raw_data.data() + pe.sections[i].PointerToRawData, 
            pe.sections[i].SizeOfRawData
        );
        if (!cpu->mem_write(section_dest, const_cast<uint8_t*>(section_slice.data()), pe.sections[i].SizeOfRawData)) {
            return false;
        }
    }

    return true;
}

bool resolve_reloc(CPU* cpu, PE pe, uintptr_t address){
    uintptr_t relocRVA  = pe.image_optional_header.BaseRelocationTableVA;
    uintptr_t relocSize = pe.image_optional_header.BaseRelocationTableSize;

    if (relocRVA == 0 || relocSize == 0) return true;

    uintptr_t currentBlockAddr = address + relocRVA;
    uintptr_t endRelocAddr = currentBlockAddr + relocSize;
    
    while(currentBlockAddr < endRelocAddr) {
        IMAGE_BASE_RELOCATION blockHeader = {0};
        if(!cpu->mem_read(currentBlockAddr, &blockHeader, sizeof(IMAGE_BASE_RELOCATION))) return false;

        if(blockHeader.SizeOfBlock == 0) break; // reached the end

        // immediatly after the header there is the block data
        size_t entryAmount = (blockHeader.SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION)) / sizeof(uint16_t);
        uintptr_t entriesStartAddr = currentBlockAddr + sizeof(IMAGE_BASE_RELOCATION);
        
        for(size_t i = 0; i < entryAmount; i++){
            // every entry is a 16 bits // WORD
            uint16_t entry;
            if(!cpu->mem_read(entriesStartAddr + (i * sizeof(uint16_t)), &entry,sizeof(uint16_t))) return false;
    
            // first 4 bits
            uint8_t type = (entry >> 12);
    
            // 12 last bits
            uint16_t offset = (entry & 0x0FFF);

            uintptr_t addr_to_patch = address + blockHeader.VirtualAddress + offset;

            if(type == 0) continue; // do nothing XD. 0 is IMAGE_REL_BASED_ABSOLUTE and gets ignored
                
            // 32 bit relocation, modify an unsigned int value of 32 bits
            else if (type == 3) {
                
                uint32_t newValue = 0;
                if(!cpu->mem_read(addr_to_patch, &newValue, sizeof(uint32_t))) return false;
    
                newValue += address - pe.image_optional_header.ImageBase;
                if(!cpu->mem_write(addr_to_patch, &newValue, sizeof(uint32_t))) return false;
            }
    
            // 64 bit relocation. modify an unsigned int value of 64 bits
            else if (type == 10) {
                uint64_t newValue = 0;
                if(!cpu->mem_read(addr_to_patch, &newValue, sizeof(uint64_t))) return false;
    
                newValue += address - pe.image_optional_header.ImageBase;
                if(!cpu->mem_write(addr_to_patch, &newValue, sizeof(uint64_t))) return false;
            }
        }
        // next block
        currentBlockAddr += blockHeader.SizeOfBlock;
    }
    
    return true;
}

// atm, im not focusing on providint ntoskrnl.exe functions, so we are putting a hook trap to every entry of the IAT
bool resolveIatWithHookTrap(CPU* cpu, PE pe, uintptr_t address){
    uintptr_t imdRVA  = pe.image_optional_header.ImportDirectoryVA;
    uintptr_t imdSize = pe.image_optional_header.ImportDirectorySize;
    if (imdRVA == 0 || imdSize == 0) return true; // no imports
    uintptr_t imdAddr = address + imdRVA;
    
    uint64_t hook = HOOK_TRAP_ADDR;
    uint64_t currentAddr = 1;

    int i = 0;
    int j = 0;
    while(true) {
        IMAGE_IMPORT_DESCRIPTOR current_import_descriptor = {};
        if(!cpu->mem_read(imdAddr + (i * sizeof(IMAGE_IMPORT_DESCRIPTOR)), &current_import_descriptor, sizeof(IMAGE_IMPORT_DESCRIPTOR))) return false;

        if(current_import_descriptor.Name == 0 && current_import_descriptor.FirstThunk == 0) break;
        uintptr_t iatAddr = current_import_descriptor.FirstThunk;

        j = 0;
        // fill iat with hooks
        while(true){
            if(!cpu->mem_read(address + iatAddr + (j * sizeof(uint64_t)), &currentAddr, sizeof(uint64_t))) return false;
            if(currentAddr == 0) break;
            
            if(cpu->mem_write(address + iatAddr + (j * sizeof(uint64_t)), &hook, sizeof(uint64_t))) return false;
            j++;
        }
        
        i++;
    }
    
    return true;
}

bool resolveIat(CPU* cpu, PE pe, uintptr_t address){
    resolveIatWithHookTrap(cpu, pe, address);
    return true;
}

bool PEManager::load_pe(CPU* cpu, PE pe, uintptr_t address){
    if(!load_headers(cpu, pe, address))  return false;
    if(!load_sections(cpu, pe, address)) return false;
    if(!resolve_reloc(cpu, pe, address)) return false;
    if(!resolveIat(cpu, pe, address))    return false;
    // mem protect configuration
    
    return true;
}