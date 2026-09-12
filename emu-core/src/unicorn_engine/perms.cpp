#include "UnicornEngine.hpp"
#include "../cpu/perms.hpp"

int UnicornEngine::to_uc_prot(uint32_t prot){
    int result = UC_PROT_NONE;
    if (has_read_perms(prot))  result |= UC_PROT_READ;
    if (has_write_perms(prot)) result |= UC_PROT_WRITE;
    if (has_exec_perms(prot))  result |= UC_PROT_EXEC;
    return result;
}