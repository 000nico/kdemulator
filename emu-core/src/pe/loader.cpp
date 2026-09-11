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
    return true;
}

bool PEManager::load_pe(CPU* cpu, PE pe, uintptr_t address){
    if(!load_headers(cpu, pe, address))  return false;
    if(!load_sections(cpu, pe, address)) return false;
    if(!resolve_reloc(cpu, pe, address)) return false;
    // resolve iat
    // mem protect configuration
    
    return true;
}