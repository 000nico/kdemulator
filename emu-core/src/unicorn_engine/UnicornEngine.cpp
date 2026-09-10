#include "UnicornEngine.hpp"
#include "unicorn/unicorn.h"

UnicornEngine::UnicornEngine() {
    uc_err err = uc_open(UC_ARCH_X86, UC_MODE_64, &uc);
    if (err != UC_ERR_OK) {
        printf("Error opening unicorn: %s\n", uc_strerror(err));
        uc = NULL;
    }
}

UnicornEngine::~UnicornEngine() {
    if (uc != NULL) {
        uc_close(uc);
    }
}
