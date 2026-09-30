#include "Api.hpp"
#include "../emu-core/src/kernel/include/structs.hpp"
#include <cstdint>
#include <string>
#include <vector>

uint64_t Api::get_arg(CPU* cpu, int index){
    switch(index) {
        case 0: return cpu->get_register(REG_RCX);
        case 1: return cpu->get_register(REG_RDX);
        case 2: return cpu->get_register(REG_R8);
        case 3: return cpu->get_register(REG_R9);
        default: {
            uint64_t rsp = cpu->get_register(REG_RSP);
            uint64_t val = 0;
            // Microsoft x64 calling convention:
            // Caller allocates 32 bytes shadow space (0x20) above the return address (at [rsp]).
            // So 5th arg (index 4) is at [rsp + 0x28], 6th (index 5) at [rsp + 0x30], etc.
            cpu->mem_read(rsp + 0x28 + (index - 4) * 8, &val, sizeof(uint64_t));
            return val;
        }
    }
}

std::string Api::read_string(CPU* cpu, uint64_t address) {
    if (address == 0) return "";
    char buf[512] = {};
    cpu->mem_read(address, buf, sizeof(buf) - 1);
    return std::string(buf);
}

std::string Api::read_unicode_string(CPU* cpu, uint64_t unicode_str_addr) {
    if (unicode_str_addr == 0) return "";
    UNICODE_STRING us{};
    if (!cpu->mem_read(unicode_str_addr, &us, sizeof(UNICODE_STRING))) {
        return "";
    }
    if (us.Length == 0 || us.Buffer == nullptr) return "";

    size_t char_count = us.Length / sizeof(wchar_t);
    if (char_count > 512) char_count = 512;
    std::vector<wchar_t> wbuf(char_count + 1, 0);
    cpu->mem_read(reinterpret_cast<uint64_t>(us.Buffer), wbuf.data(), char_count * sizeof(wchar_t));

    std::string out;
    out.reserve(char_count);
    for (size_t i = 0; i < char_count && wbuf[i] != 0; ++i) {
        wchar_t c = wbuf[i];
        if (c < 128) out.push_back(static_cast<char>(c));
        else out.push_back('?');
    }
    return out;
}

std::string Api::read_ansi_string(CPU* cpu, uint64_t ansi_str_addr) {
    if (ansi_str_addr == 0) return "";
    struct {
        uint16_t Length;
        uint16_t MaximumLength;
        uint32_t Pad;
        uint64_t Buffer;
    } as{};
    if (!cpu->mem_read(ansi_str_addr, &as, sizeof(as))) {
        return "";
    }
    if (as.Length == 0 || as.Buffer == 0) return "";
    size_t len = as.Length > 512 ? 512 : as.Length;
    std::vector<char> buf(len + 1, 0);
    cpu->mem_read(as.Buffer, buf.data(), len);
    return std::string(buf.data());
}

std::wstring Api::read_wide_string(CPU* cpu, uint64_t address, size_t max_chars) {
    if (address == 0) return L"";
    std::wstring result;
    uint64_t curr = address;
    for (size_t i = 0; i < max_chars; ++i) {
        wchar_t ch = 0;
        if (!cpu->mem_read(curr, &ch, sizeof(wchar_t)) || ch == 0) {
            break;
        }
        result += ch;
        curr += sizeof(wchar_t);
    }
    return result;
}

uint32_t Api::read_u32(CPU* cpu, uint64_t address) {
    if (address == 0) return 0;
    uint32_t val = 0;
    cpu->mem_read(address, &val, sizeof(val));
    return val;
}

uint64_t Api::read_u64(CPU* cpu, uint64_t address) {
    if (address == 0) return 0;
    uint64_t val = 0;
    cpu->mem_read(address, &val, sizeof(val));
    return val;
}

bool Api::write_u32(CPU* cpu, uint64_t address, uint32_t val) {
    if (address == 0) return false;
    return cpu->mem_write(address, &val, sizeof(val));
}

bool Api::write_u64(CPU* cpu, uint64_t address, uint64_t val) {
    if (address == 0) return false;
    return cpu->mem_write(address, &val, sizeof(val));
}