#include "unicorn/unicorn.h"
#include "UnicornEngine.hpp"
#include "../memory/layout.hpp"
#include "../debug/Debug.hpp"



UnicornEngine::UnicornEngine() {
    uc_err err = uc_open(UC_ARCH_X86, UC_MODE_64, &uc);
    if (err != UC_ERR_OK) {
        Debug::debug_msg(std::string("Error opening unicorn: ") + uc_strerror(err), LOG_ERROR);
        uc = NULL;
    }
}

UnicornEngine::~UnicornEngine() {
    if (uc != NULL) {
        uc_close(uc);
    }
}

uintptr_t UnicornEngine::get_stack_base() {
    return STACK_BASE;
}

size_t UnicornEngine::get_stack_size() {
    return STACK_SIZE;
}

uintptr_t UnicornEngine::get_heap_base() {
    return HEAP_BASE;
}

size_t UnicornEngine::get_heap_size() {
    return HEAP_SIZE;
}

