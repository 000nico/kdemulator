#include "UnicornEngine.hpp"
#include "unicorn/unicorn.h"

bool UnicornEngine::mem_map(uintptr_t address, size_t size, uint32_t perms){
    perms = to_uc_prot(perms);
    return uc_mem_map(this->uc, address, size, perms) == UC_ERR_OK; 
}

bool UnicornEngine::apply_mem_prot(uintptr_t address, size_t size, uint32_t perms) {
    perms = to_uc_prot(perms);
    return uc_mem_protect(this->uc, address, size, perms) == UC_ERR_OK;
}

bool UnicornEngine::mem_read(uintptr_t address, void *buffer, size_t size){
    return uc_mem_read(this->uc, address, buffer, size) == UC_ERR_OK;
}

bool UnicornEngine::mem_write(uintptr_t address, void *data, size_t size){
    return uc_mem_write(this->uc, address, data, size) == UC_ERR_OK;
}