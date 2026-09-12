#pragma once
#include <cstdint>

#define PROT_NONE  0
#define PROT_READ  (1 << 0)
#define PROT_WRITE (1 << 1)
#define PROT_EXEC  (1 << 2)
#define PROT_ALL   (PROT_READ | PROT_WRITE | PROT_EXEC)

inline bool has_read_perms(uint32_t prot) {
    return (prot & PROT_READ) != 0;
}

inline bool has_write_perms(uint32_t prot) {
    return (prot & PROT_WRITE) != 0;
}

inline bool has_exec_perms(uint32_t prot) {
    return (prot & PROT_EXEC) != 0;
}