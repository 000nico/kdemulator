#include <unicorn/unicorn.h>
#include <cstdio>

int main() {
    uc_engine* uc;
    uc_err err = uc_open(UC_ARCH_X86, UC_MODE_64, &uc);
    if (err != UC_ERR_OK) {
        printf("Error al abrir Unicorn: %s\n", uc_strerror(err));
        return 1;
    }
    printf("Unicorn OK - version %u.%u\n", UC_VERSION_MAJOR, UC_VERSION_MINOR);
    uc_close(uc);
    return 0;
}