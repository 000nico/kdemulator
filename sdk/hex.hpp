#pragma once
#include <string>
#include <sstream>
#include <cstdint>

inline std::string hex32(uint32_t v) {
    std::ostringstream oss;
    oss << "0x" << std::uppercase << std::hex << v;
    return oss.str();
}

inline std::string hex64(uint64_t v) {
    std::ostringstream oss;
    oss << "0x" << std::uppercase << std::hex << v;
    return oss.str();
}
