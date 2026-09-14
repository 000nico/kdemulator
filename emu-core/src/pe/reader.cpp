#include <cstdio>
#include <cstdlib>
#include "PEManager.hpp"
#include "../sdk/hex.hpp"
#include "../debug/Debug.hpp"

long getFileSize(FILE* file) {
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    return file_size;
}

bool PEManager::read_pe_from_disk(char* path, PE* pe) {
    Debug::debug_msg(std::string("read_pe: opening ") + path + "\n", LOG_INFO);

    FILE* file = fopen(path, "rb");
    if (!file) {
        Debug::debug_msg("read_pe: failed to open file\n", LOG_ERROR);
        return false;
    }

    long file_size = getFileSize(file);
    Debug::debug_msg("read_pe: file_size = " + std::to_string(file_size) + " bytes\n", LOG_INFO);

    if (file_size <= 0) {
        fclose(file);
        return false;
    }

    pe->raw_data.resize(file_size);
    if (fread(pe->raw_data.data(), 1, file_size, file) != (size_t)file_size) {
        fclose(file);
        return false;
    }
    fclose(file);

    if (pe->raw_data.size() < sizeof(IMAGE_DOS_HEADER)) return false;
    IMAGE_DOS_HEADER* dos = (IMAGE_DOS_HEADER*)pe->raw_data.data();
    if (dos->e_magic != 0x5A4D) return false; // 'MZ'
    pe->image_dos_header = *dos;
    Debug::debug_msg("read_pe: DOS magic = " + hex32(dos->e_magic) + "  e_lfanew = " + hex32(dos->e_lfanew) + "\n", LOG_INFO);
    
    if (dos->e_lfanew + sizeof(IMAGE_FILE_HEADER) > pe->raw_data.size()) return false;
    uint8_t* nt = pe->raw_data.data() + dos->e_lfanew;

    IMAGE_FILE_HEADER* file_hdr = (IMAGE_FILE_HEADER*)nt;
    if (file_hdr->Signature != 0x00004550) return false; // 'PE\0\0'
    pe->image_file_header = *file_hdr;
    Debug::debug_msg("read_pe: Machine = " + hex32(file_hdr->Machine) + "  Sections = " + std::to_string(file_hdr->NumberOfSections) + "\n", LOG_INFO);

    IMAGE_OPTIONAL_HEADER* opt_hdr = (IMAGE_OPTIONAL_HEADER*)(nt + sizeof(IMAGE_FILE_HEADER));
    if (opt_hdr->Magic != 0x020B) return false; // PE32+ (x64)
    pe->image_optional_header = *opt_hdr;
    Debug::debug_msg("read_pe: ImageBase    = " + hex64(opt_hdr->ImageBase) + "\n", LOG_INFO);
    Debug::debug_msg("read_pe: EntryPoint   = " + hex32(opt_hdr->AddressOfEntryPoint) + "\n", LOG_INFO);
    Debug::debug_msg("read_pe: SizeOfImage  = " + hex32(opt_hdr->SizeOfImage) + "\n", LOG_INFO);

    IMAGE_SECTION_HEADER* sec_array = (IMAGE_SECTION_HEADER*)((uint8_t*)opt_hdr + file_hdr->SizeOfOptionalHeader);

    pe->sections.resize(file_hdr->NumberOfSections);
    for (uint16_t i = 0; i < file_hdr->NumberOfSections; i++) {
        pe->sections[i] = sec_array[i];
        std::string name(reinterpret_cast<char*>(sec_array[i].Name), 8);
        name = name.substr(0, name.find('\0')); // trim null padding
        Debug::debug_msg(
            "read_pe: section [" + name + "]"
            "  VA=" + hex32(sec_array[i].VirtualAddress) +
            "  raw=" + hex32(sec_array[i].PointerToRawData) +
            "  size=" + hex32(sec_array[i].SizeOfRawData) + "\n",
            LOG_INFO
        );
    }

    Debug::debug_msg("read_pe: PE parsed successfully\n", LOG_INFO);
    return true;
}