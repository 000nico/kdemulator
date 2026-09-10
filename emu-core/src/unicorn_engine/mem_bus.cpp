#include "UnicornEngine.hpp"
#include "unicorn/unicorn.h"

bool UnicornEngine::mem_map(uintptr_t address, size_t size){
    return uc_mem_map(this->uc, address, size, uint32_t perms) == UC_ERR_OK; // todo ver esto de hacer mis propios perms
}

bool UnicornEngine::mem_read(uintptr_t address, void *buffer, size_t size){
    return uc_mem_read(this->uc, address, buffer, size);
}

bool UnicornEngine::mem_write(uintptr_t address, void *data, size_t size){
    return uc_mem_write(this->uc, address, data, size);
}