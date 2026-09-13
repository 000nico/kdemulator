#include "PEManager.hpp"
#include "../memory/layout.hpp"
#include "../cpu/perms.hpp"
#include <cstdint>
#include <span>
#include <sstream>
#include "../debug/Debug.hpp"

uint64_t align_up(uint64_t value, uint64_t alignment) {
    return (value + alignment - 1) & ~(alignment - 1);
}

static std::string hex64(uint64_t v) {
    std::ostringstream oss;
    oss << "0x" << std::uppercase << std::hex << v;
    return oss.str();
}

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
            std::string name(reinterpret_cast<char*>(pe.sections[i].Name), 8);
            name = name.substr(0, name.find('\0'));
            Debug::debug_msg("load_sections: mem_write failed on section [" + name + "] dest=" + hex64(section_dest) + "\n", LOG_ERROR);
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
        if(!cpu->mem_read(imdAddr + (i * sizeof(IMAGE_IMPORT_DESCRIPTOR)), &current_import_descriptor, sizeof(IMAGE_IMPORT_DESCRIPTOR))) {
            Debug::debug_msg("resolveIat: mem_read failed reading import descriptor " + std::to_string(i) + "\n", LOG_ERROR);
            return false;
        }

        if(current_import_descriptor.Name == 0 && current_import_descriptor.FirstThunk == 0) break;
        uintptr_t iatAddr = current_import_descriptor.FirstThunk;

        j = 0;
        // fill iat with hooks
        while(true){
            if(!cpu->mem_read(address + iatAddr + (j * sizeof(uint64_t)), &currentAddr, sizeof(uint64_t))) {
                Debug::debug_msg("resolveIat: mem_read failed reading IAT entry " + std::to_string(j) + " (descriptor " + std::to_string(i) + ")\n", LOG_ERROR);
                return false;
            }
            if(currentAddr == 0) break;
            
            if(!cpu->mem_write(address + iatAddr + (j * sizeof(uint64_t)), &hook, sizeof(uint64_t))) {
                Debug::debug_msg("resolveIat: mem_write failed writing hook at IAT entry " + std::to_string(j) + " (descriptor " + std::to_string(i) + ")\n", LOG_ERROR);
                return false;
            }
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

bool PEManager::load_pe(CPU* cpu, PE pe, uintptr_t address){
    if(!load_headers(cpu, pe, address))  return false;
    Debug::debug_msg("pe loader: headers loaded\n", LOG_INFO);
    if(!load_sections(cpu, pe, address)) return false;
    Debug::debug_msg("pe loader: sections loaded\n", LOG_INFO);
    if(!resolve_reloc(cpu, pe, address)) return false;
    Debug::debug_msg("pe loader: .relocs solved\n", LOG_INFO);
    if(!resolveIat(cpu, pe, address))    return false;
    Debug::debug_msg("pe loader: iat resolved\n", LOG_INFO);
    if(!applyMemProtect(cpu,pe, address))return false;
    Debug::debug_msg("pe loader: mem protections applied\n", LOG_INFO);
    
    return true;
}
