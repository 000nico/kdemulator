#include "loader.hpp"
#include "../../memory/layout.hpp"
#include "../../debug/Debug.hpp"

static std::string read_module_name(CPU* cpu, uintptr_t address, uint32_t name_rva) {
    std::string module_name = "";
    uintptr_t dllNameRvaAddr = address + name_rva;
    char ch = 0;
    int charIndex = 0;

    while(cpu->mem_read(dllNameRvaAddr + charIndex, &ch, sizeof(char)) && ch != '\0'){
        module_name += ch;
        charIndex++;
    }
    return module_name;
}

static std::string read_function_name(CPU* cpu, uintptr_t address, uint64_t address_of_data) {
    uintptr_t importByNameAddr = address + address_of_data;
    std::string function_name = "";
    int nameIndex = 0;
    char nameChar = 0;
    while (cpu->mem_read(importByNameAddr + 2 + nameIndex, &nameChar, sizeof(char)) && nameChar != '\0') {
        function_name += nameChar;
        nameIndex++;
    }
    return function_name;
}

static bool resolve_import_thunks(CPU* cpu, uintptr_t address, const IMAGE_IMPORT_DESCRIPTOR& current_import_descriptor, int i, const std::string& module_name, std::vector<TrapEntry>* trap_table) {
    uintptr_t iatAddr = current_import_descriptor.FirstThunk;
    uintptr_t iltAddr = current_import_descriptor.OriginalFirstThunk ? current_import_descriptor.OriginalFirstThunk : iatAddr;
    uint64_t hook_base = HOOK_TRAP_BASE;
    uint64_t currentAddr = 1;
    int j = 0;
    
    while(true){
        IMAGE_THUNK_DATA64 thunk = {};
        if (!cpu->mem_read(address + iltAddr + (j * sizeof(IMAGE_THUNK_DATA64)), &thunk, sizeof(IMAGE_THUNK_DATA64))) {
            return false;
        }

        if(thunk.u1.Function == 0) break;

        std::string function_name = read_function_name(cpu, address, thunk.u1.AddressOfData);
        
        if(!cpu->mem_read(address + iatAddr + (j * sizeof(uint64_t)), &currentAddr, sizeof(uint64_t))) {
            Debug::debug_msg("resolveIat: mem_read failed reading IAT entry " + std::to_string(j) + " (descriptor " + std::to_string(i) + ")\n", LOG_ERROR);
            return false;
        }
        
        if(currentAddr == 0) break;

        uint64_t target_hook_addr = hook_base + (((i * 1000) + j) * 0x10);
        if(!cpu->mem_write(address + iatAddr + (j * sizeof(uint64_t)), &target_hook_addr, sizeof(uint64_t))) {
            Debug::debug_msg("resolveIat: mem_write failed writing hook at IAT entry " + std::to_string(j) + " (descriptor " + std::to_string(i) + ")\n", LOG_ERROR);
            return false;
        }

        if (trap_table != nullptr) {
            TrapEntry trap_entry;
            trap_entry.module_name = module_name;
            trap_entry.function_name = function_name;
            trap_table->push_back(trap_entry);
        }

        j++;
    }
    return true;
}

static bool resolve_import_descriptor(CPU* cpu, uintptr_t address, uintptr_t imdAddr, int i, std::vector<TrapEntry>* trap_table, bool& is_end) {
    IMAGE_IMPORT_DESCRIPTOR current_import_descriptor = {};
    if(!cpu->mem_read(imdAddr + (i * sizeof(IMAGE_IMPORT_DESCRIPTOR)), &current_import_descriptor, sizeof(IMAGE_IMPORT_DESCRIPTOR))) {
        Debug::debug_msg("resolveIat: mem_read failed reading import descriptor " + std::to_string(i) + "\n", LOG_ERROR);
        return false;
    }

    if(current_import_descriptor.Name == 0 && current_import_descriptor.FirstThunk == 0) {
        is_end = true;
        return true;
    }
    is_end = false;

    std::string module_name = read_module_name(cpu, address, current_import_descriptor.Name);

    return resolve_import_thunks(cpu, address, current_import_descriptor, i, module_name, trap_table);
}

// Every function gets manually implemented. It can be a full implementation of the function or just a fake return 0, but every one gets implemented
// So every one is hooked to a different address to be recognized and dispatched by emu-stubs
bool resolveIatWithHookTrap(CPU* cpu, PE pe, uintptr_t address, std::vector<TrapEntry>* trap_table){
    uintptr_t imdRVA  = pe.image_optional_header.ImportDirectoryVA;
    uintptr_t imdSize = pe.image_optional_header.ImportDirectorySize;
    if (imdRVA == 0 || imdSize == 0) return true;
    
    uintptr_t imdAddr = address + imdRVA;

    int i = 0;
    while(true) {
        bool is_end = false;
        if(!resolve_import_descriptor(cpu, address, imdAddr, i, trap_table, is_end)) {
            return false;
        }
        if(is_end) break;
        i++;
    }
    
    return true;
}

bool resolveIat(CPU* cpu, PE pe, uintptr_t address, std::vector<TrapEntry>* trap_table){
    return resolveIatWithHookTrap(cpu, pe, address, trap_table);
}
