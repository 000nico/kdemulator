#include "loader.hpp"
#include "../../debug/Debug.hpp"
#include <span>

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
