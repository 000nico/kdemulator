#include <cstdio>
#include <cstdlib>
#include "PEManager.hpp"

bool PEManager::read_pe_from_disk(char* path, PE* pe) {
    FILE* file = fopen(path, "rb");
    if (!file) return false;

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

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
    
    if (dos->e_lfanew + sizeof(IMAGE_FILE_HEADER) > pe->raw_data.size()) return false;
    uint8_t* nt = pe->raw_data.data() + dos->e_lfanew;

    IMAGE_FILE_HEADER* file_hdr = (IMAGE_FILE_HEADER*)nt;
    if (file_hdr->Signature != 0x00004550) return false; // 'PE\0\0'
    pe->image_file_header = *file_hdr;

    IMAGE_OPTIONAL_HEADER* opt_hdr = (IMAGE_OPTIONAL_HEADER*)(nt + sizeof(IMAGE_FILE_HEADER));
    if (opt_hdr->Magic != 0x020B) return false; // PE32+ (x64)
    pe->image_optional_header = *opt_hdr;

    IMAGE_SECTION_HEADER* sec_array = (IMAGE_SECTION_HEADER*)((uint8_t*)opt_hdr + file_hdr->SizeOfOptionalHeader);

    pe->sections.resize(file_hdr->NumberOfSections);
    for (uint16_t i = 0; i < file_hdr->NumberOfSections; i++) {
        pe->sections[i] = sec_array[i];
    }

    return true;
}