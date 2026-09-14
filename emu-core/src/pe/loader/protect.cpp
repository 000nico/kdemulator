#include "loader.hpp"
#include "../../cpu/perms.hpp"
#include "../../debug/Debug.hpp"

uint32_t section_to_prot(uint32_t characteristics) {
    uint32_t prot = PROT_NONE;
    if (characteristics & IMAGE_SCN_MEM_READ)    prot |= PROT_READ;
    if (characteristics & IMAGE_SCN_MEM_WRITE)   prot |= PROT_WRITE;
    if (characteristics & IMAGE_SCN_MEM_EXECUTE) prot |= PROT_EXEC;
    return prot;
}

bool applyMemProtect(CPU* cpu, PE pe, uintptr_t address){
    for(size_t i = 0; i < pe.sections.size(); i++){
        uintptr_t section_va = address + pe.sections[i].VirtualAddress;
        
        uint32_t virtual_size = pe.sections[i].PhysicalAddress_VirtualSize;
        if (virtual_size == 0) virtual_size = pe.sections[i].SizeOfRawData;
        uint32_t section_size = align_up(virtual_size, 0x1000);
        
        uint32_t prot = section_to_prot(pe.sections[i].Characteristics);

        if(!cpu->apply_mem_prot(section_va, section_size, prot)) {
            std::string name(reinterpret_cast<char*>(pe.sections[i].Name), 8);
            name = name.substr(0, name.find('\0'));
            Debug::debug_msg("applyMemProtect: failed applying memory protections on [" + name + "] va=" + hex64(section_va) + " size=" + hex64(section_size) + "\n", LOG_ERROR);
            return false;
        }
    } 

    return true;
}
