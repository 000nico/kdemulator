#include "loader.hpp"
#include "../../cpu/perms.hpp"
#include "../../debug/Debug.hpp"

bool load_headers(CPU* cpu, PE pe, uintptr_t address){
    if(!cpu->mem_map(address, pe.image_optional_header.SizeOfImage, PROT_ALL)) {
        Debug::debug_msg("load_headers: mem_map failed at " + hex64(address) + " size=" + hex64(pe.image_optional_header.SizeOfImage) + "\n", LOG_ERROR);
        return false;
    }

    uintptr_t map_in_addr = address;
    if(!cpu->mem_write(map_in_addr, &pe.image_dos_header, sizeof(pe.image_dos_header))) {
        Debug::debug_msg("load_headers: mem_write failed writing DOS header at " + hex64(map_in_addr) + "\n", LOG_ERROR);
        return false;
    }

    map_in_addr += pe.image_dos_header.e_lfanew;
    
    if(!cpu->mem_write(map_in_addr, &pe.image_file_header, sizeof(pe.image_file_header))) {
        Debug::debug_msg("load_headers: mem_write failed writing FILE header at " + hex64(map_in_addr) + "\n", LOG_ERROR);
        return false;
    }
    map_in_addr += sizeof(pe.image_file_header);
    
    if(!cpu->mem_write(map_in_addr, &pe.image_optional_header, sizeof(pe.image_optional_header))) {
        Debug::debug_msg("load_headers: mem_write failed writing OPTIONAL header at " + hex64(map_in_addr) + "\n", LOG_ERROR);
        return false;
    }
    map_in_addr += sizeof(pe.image_optional_header);
    
    for(int i = 0; i < (int)pe.sections.size(); i++){
        if(!cpu->mem_write(map_in_addr, &pe.sections[i], sizeof(pe.sections[i]))) {
            std::string name(reinterpret_cast<char*>(pe.sections[i].Name), 8);
            name = name.substr(0, name.find('\0'));
            Debug::debug_msg("load_headers: mem_write failed writing section header [" + name + "] at " + hex64(map_in_addr) + "\n", LOG_ERROR);
            return false;
        }
        map_in_addr += sizeof(IMAGE_SECTION_HEADER);
    }
    
    return true;
}
