#include <cstdio>
#include <cstdlib>
#include "format.hpp"

bool read_pe_from_disk(char* path, PE* pe){
    FILE* file = fopen(path, "rb");
    if(!file) return false;

    IMAGE_DOS_HEADER image_dos_header;
    fread(&image_dos_header, sizeof(image_dos_header), 1, file);

    if(image_dos_header.e_magic != 0x5A4D) {
        fclose(file);
        return false;
    }

    fseek(file, image_dos_header.e_lfanew, SEEK_SET);

    unsigned int signature;
    fread(&signature, sizeof(unsigned int), 1, file);

    if(signature != 0x00004550) {
        fclose(file);
        return false;
    }

    IMAGE_FILE_HEADER image_file_header;
    fread(&image_file_header, sizeof(image_file_header), 1, file);

    IMAGE_OPTIONAL_HEADER image_optional_header;
    fread(&image_optional_header, sizeof(image_optional_header), 1, file);

    IMAGE_SECTION_HEADER* sections = new IMAGE_SECTION_HEADER[image_file_header.NumberOfSections];
    size_t read_count = fread(sections, sizeof(IMAGE_SECTION_HEADER), image_file_header.NumberOfSections, file);
    
    if(read_count != image_file_header.NumberOfSections){
        delete[] sections;
        fclose(file);
        return false;
    }

    pe->image_dos_header = image_dos_header;
    pe->image_file_header = image_file_header;
    pe->image_optional_header = image_optional_header;
    pe->image_section_header = sections;

    fclose(file);
    return true;
}