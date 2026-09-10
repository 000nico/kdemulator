#include "UnicornEngine.hpp"
#include "unicorn/unicorn.h"

int UnicornEngine::get_register(Register reg) {
    switch (reg) {
        case REG_RAX:    return UC_X86_REG_RAX;
        case REG_RBX:    return UC_X86_REG_RBX;
        case REG_RCX:    return UC_X86_REG_RCX;
        case REG_RDX:    return UC_X86_REG_RDX;
        case REG_RSI:    return UC_X86_REG_RSI;
        case REG_RDI:    return UC_X86_REG_RDI;
        case REG_RSP:    return UC_X86_REG_RSP;
        case REG_RBP:    return UC_X86_REG_RBP;
        case REG_R8:     return UC_X86_REG_R8;
        case REG_R9:     return UC_X86_REG_R9;
        case REG_R10:    return UC_X86_REG_R10;
        case REG_R11:    return UC_X86_REG_R11;
        case REG_R12:    return UC_X86_REG_R12;
        case REG_R13:    return UC_X86_REG_R13;
        case REG_R14:    return UC_X86_REG_R14;
        case REG_R15:    return UC_X86_REG_R15;
        case REG_RIP:    return UC_X86_REG_RIP;
        case REG_RFLAGS: return UC_X86_REG_EFLAGS;
    }

    return -1;
}

bool UnicornEngine::set_register(Register reg, uint64_t value) {
    return uc_reg_write(this->uc, get_register(reg), &value) == UC_ERR_OK;
}
