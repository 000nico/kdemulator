#pragma once
#include "../Api.hpp"
#include "../memory/allocators_common.hpp"
#include "../../../emu-core/src/debug/Debug.hpp"
#include "../../../sdk/hex.hpp"
#include <string>
#include <vector>
#include <cwctype>
#include <algorithm>

// RtlGetVersion(PRTL_OSVERSIONINFOW lpVersionInformation) -> NTSTATUS
class ApiRtlGetVersion : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t info_ptr = get_arg(cpu, 0);
        if (info_ptr != 0) {
            uint32_t size = read_u32(cpu, info_ptr);
            write_u32(cpu, info_ptr + 4, 10);    // Major = 10
            write_u32(cpu, info_ptr + 8, 0);     // Minor = 0
            write_u32(cpu, info_ptr + 12, 19045); // Build = 19045 (22H2)
            write_u32(cpu, info_ptr + 16, 2);    // PlatformId = VER_PLATFORM_WIN32_NT

            // Zero out szCSDVersion (128 WCHARs = 256 bytes)
            std::vector<uint8_t> zeros(256, 0);
            cpu->mem_write(info_ptr + 20, zeros.data(), 256);

            // If EX struct
            if (size >= 284) {
                uint16_t sp_major = 0;
                uint16_t sp_minor = 0;
                uint16_t suite_mask = 0x0100;
                uint8_t  prod_type  = 1; // VER_NT_WORKSTATION
                cpu->mem_write(info_ptr + 276, &sp_major, 2);
                cpu->mem_write(info_ptr + 278, &sp_minor, 2);
                cpu->mem_write(info_ptr + 280, &suite_mask, 2);
                cpu->mem_write(info_ptr + 282, &prod_type, 1);
            }
        }
        Debug::debug_msg("[RtlGetVersion] -> Windows 10 Build 19045", LOG_INFO);
        return 0; // STATUS_SUCCESS
    }
};

// RtlCompareMemory(const VOID* Source1, const VOID* Source2, SIZE_T Length) -> SIZE_T
class ApiRtlCompareMemory : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t src1 = get_arg(cpu, 0);
        uint64_t src2 = get_arg(cpu, 1);
        size_t   len  = static_cast<size_t>(get_arg(cpu, 2));

        size_t matched = 0;
        for (size_t i = 0; i < len; ++i) {
            uint8_t b1 = 0, b2 = 0;
            if (!cpu->mem_read(src1 + i, &b1, 1) || !cpu->mem_read(src2 + i, &b2, 1)) break;
            if (b1 != b2) break;
            matched++;
        }

        Debug::debug_msg("[RtlCompareMemory] len=" + std::to_string(len) + " matched=" + std::to_string(matched), LOG_INFO);
        return matched;
    }
};

// RtlEqualMemory(const VOID* Source1, const VOID* Source2, SIZE_T Length) -> BOOLEAN
class ApiRtlEqualMemory : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t src1 = get_arg(cpu, 0);
        uint64_t src2 = get_arg(cpu, 1);
        size_t   len  = static_cast<size_t>(get_arg(cpu, 2));

        bool eq = true;
        for (size_t i = 0; i < len; ++i) {
            uint8_t b1 = 0, b2 = 0;
            if (!cpu->mem_read(src1 + i, &b1, 1) || !cpu->mem_read(src2 + i, &b2, 1) || b1 != b2) {
                eq = false;
                break;
            }
        }

        Debug::debug_msg("[RtlEqualMemory] len=" + std::to_string(len) + " -> " + (eq ? "TRUE" : "FALSE"), LOG_INFO);
        return eq ? 1 : 0;
    }
};

// RtlFillMemory(VOID* Destination, SIZE_T Length, UCHAR Fill)
class ApiRtlFillMemory : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t dst  = get_arg(cpu, 0);
        size_t   len  = static_cast<size_t>(get_arg(cpu, 1));
        uint8_t  fill = static_cast<uint8_t>(get_arg(cpu, 2));

        std::vector<uint8_t> buf(len, fill);
        cpu->mem_write(dst, buf.data(), len);

        Debug::debug_msg("[RtlFillMemory] dst=" + hex64(dst) + " len=" + std::to_string(len) + " fill=" + hex32(fill), LOG_INFO);
        return 0;
    }
};

// RtlMoveMemory(VOID* Destination, const VOID* Source, SIZE_T Length)
class ApiRtlMoveMemory : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t dst = get_arg(cpu, 0);
        uint64_t src = get_arg(cpu, 1);
        size_t   len = static_cast<size_t>(get_arg(cpu, 2));

        std::vector<uint8_t> buf(len);
        cpu->mem_read(src, buf.data(), len);
        cpu->mem_write(dst, buf.data(), len);

        Debug::debug_msg("[RtlMoveMemory] " + hex64(src) + " -> " + hex64(dst) + " len=" + std::to_string(len), LOG_INFO);
        return 0;
    }
};

// RtlCompareUnicodeString(PCUNICODE_STRING String1, PCUNICODE_STRING String2, BOOLEAN CaseInSensitive) -> LONG
class ApiRtlCompareUnicodeString : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t s1_addr = get_arg(cpu, 0);
        uint64_t s2_addr = get_arg(cpu, 1);
        uint8_t  case_in = static_cast<uint8_t>(get_arg(cpu, 2));

        std::string s1 = read_unicode_string(cpu, s1_addr);
        std::string s2 = read_unicode_string(cpu, s2_addr);

        int res = 0;
        if (case_in) {
            std::string l1 = s1, l2 = s2;
            std::transform(l1.begin(), l1.end(), l1.begin(), ::tolower);
            std::transform(l2.begin(), l2.end(), l2.begin(), ::tolower);
            res = l1.compare(l2);
        } else {
            res = s1.compare(s2);
        }

        Debug::debug_msg("[RtlCompareUnicodeString] \"" + s1 + "\" vs \"" + s2 + "\" -> " + std::to_string(res), LOG_INFO);
        return static_cast<uint64_t>(res);
    }
};

// RtlEqualUnicodeString(PCUNICODE_STRING String1, PCUNICODE_STRING String2, BOOLEAN CaseInSensitive) -> BOOLEAN
class ApiRtlEqualUnicodeString : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t s1_addr = get_arg(cpu, 0);
        uint64_t s2_addr = get_arg(cpu, 1);
        uint8_t  case_in = static_cast<uint8_t>(get_arg(cpu, 2));

        std::string s1 = read_unicode_string(cpu, s1_addr);
        std::string s2 = read_unicode_string(cpu, s2_addr);

        bool eq = false;
        if (case_in) {
            std::string l1 = s1, l2 = s2;
            std::transform(l1.begin(), l1.end(), l1.begin(), ::tolower);
            std::transform(l2.begin(), l2.end(), l2.begin(), ::tolower);
            eq = (l1 == l2);
        } else {
            eq = (s1 == s2);
        }

        Debug::debug_msg("[RtlEqualUnicodeString] \"" + s1 + "\" == \"" + s2 + "\" -> " + (eq ? "TRUE" : "FALSE"), LOG_INFO);
        return eq ? 1 : 0;
    }
};

// RtlUpcaseUnicodeString(PUNICODE_STRING DestinationString, PCUNICODE_STRING SourceString, BOOLEAN AllocateDestinationString)
class ApiRtlUpcaseUnicodeString : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t dst_addr = get_arg(cpu, 0);
        uint64_t src_addr = get_arg(cpu, 1);
        uint8_t  alloc_dst = static_cast<uint8_t>(get_arg(cpu, 2));

        std::string src = read_unicode_string(cpu, src_addr);
        std::transform(src.begin(), src.end(), src.begin(), ::toupper);

        size_t byte_len = src.size() * sizeof(wchar_t);
        uint64_t buf_va = 0;
        if (alloc_dst) {
            buf_va = allocate(byte_len + 2, 0, 0x536E5572, cpu); // 'rUnS'
        } else {
            buf_va = read_u64(cpu, dst_addr + 8);
        }

        std::vector<wchar_t> wbuf(src.size() + 1);
        for (size_t i = 0; i < src.size(); ++i) wbuf[i] = static_cast<wchar_t>(src[i]);
        wbuf[src.size()] = 0;

        cpu->mem_write(buf_va, wbuf.data(), (src.size() + 1) * sizeof(wchar_t));

        if (dst_addr != 0) {
            write_u32(cpu, dst_addr, static_cast<uint32_t>(byte_len) | (static_cast<uint32_t>(byte_len + 2) << 16));
            write_u64(cpu, dst_addr + 8, buf_va);
        }

        Debug::debug_msg("[RtlUpcaseUnicodeString] -> \"" + src + "\"", LOG_INFO);
        return 0; // STATUS_SUCCESS
    }
};

// RtlFreeUnicodeString(PUNICODE_STRING UnicodeString)
class ApiRtlFreeUnicodeString : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t str_addr = get_arg(cpu, 0);
        if (str_addr != 0) {
            write_u32(cpu, str_addr, 0);
            write_u64(cpu, str_addr + 8, 0);
        }
        Debug::debug_msg("[RtlFreeUnicodeString] str=" + hex64(str_addr), LOG_INFO);
        return 0;
    }
};

// RtlInitAnsiString(PANSI_STRING DestinationString, PCSZ SourceString)
class ApiRtlInitAnsiString : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t dst_addr = get_arg(cpu, 0);
        uint64_t src_addr = get_arg(cpu, 1);

        std::string src = read_string(cpu, src_addr);
        uint16_t len = static_cast<uint16_t>(src.size());

        if (dst_addr != 0) {
            write_u32(cpu, dst_addr, len | ((len + 1) << 16));
            write_u64(cpu, dst_addr + 8, src_addr);
        }

        Debug::debug_msg("[RtlInitAnsiString] text=\"" + src + "\"", LOG_INFO);
        return 0;
    }
};

// RtlAnsiStringToUnicodeString(PUNICODE_STRING DestinationString, PCANSI_STRING SourceString, BOOLEAN AllocateDestinationString)
class ApiRtlAnsiStringToUnicodeString : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t dst_addr  = get_arg(cpu, 0);
        uint64_t src_addr  = get_arg(cpu, 1);
        uint8_t  alloc_dst = static_cast<uint8_t>(get_arg(cpu, 2));

        std::string src = read_ansi_string(cpu, src_addr);
        size_t byte_len = src.size() * sizeof(wchar_t);
        uint64_t buf_va = 0;
        if (alloc_dst) {
            buf_va = allocate(byte_len + 2, 0, 0x536E5572, cpu);
        } else {
            buf_va = read_u64(cpu, dst_addr + 8);
        }

        std::vector<wchar_t> wbuf(src.size() + 1);
        for (size_t i = 0; i < src.size(); ++i) wbuf[i] = static_cast<wchar_t>(src[i]);
        wbuf[src.size()] = 0;

        cpu->mem_write(buf_va, wbuf.data(), (src.size() + 1) * sizeof(wchar_t));

        if (dst_addr != 0) {
            write_u32(cpu, dst_addr, static_cast<uint32_t>(byte_len) | (static_cast<uint32_t>(byte_len + 2) << 16));
            write_u64(cpu, dst_addr + 8, buf_va);
        }

        Debug::debug_msg("[RtlAnsiStringToUnicodeString] text=\"" + src + "\"", LOG_INFO);
        return 0; // STATUS_SUCCESS
    }
};

// RtlUnicodeStringToAnsiString(PANSI_STRING DestinationString, PCUNICODE_STRING SourceString, BOOLEAN AllocateDestinationString)
class ApiRtlUnicodeStringToAnsiString : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t dst_addr  = get_arg(cpu, 0);
        uint64_t src_addr  = get_arg(cpu, 1);
        uint8_t  alloc_dst = static_cast<uint8_t>(get_arg(cpu, 2));

        std::string src = read_unicode_string(cpu, src_addr);
        size_t len = src.size();
        uint64_t buf_va = 0;
        if (alloc_dst) {
            buf_va = allocate(len + 1, 0, 0x536E4172, cpu); // 'rAnS'
        } else {
            buf_va = read_u64(cpu, dst_addr + 8);
        }

        cpu->mem_write(buf_va, (void*)src.c_str(), len + 1);

        if (dst_addr != 0) {
            write_u32(cpu, dst_addr, static_cast<uint32_t>(len) | (static_cast<uint32_t>(len + 1) << 16));
            write_u64(cpu, dst_addr + 8, buf_va);
        }

        Debug::debug_msg("[RtlUnicodeStringToAnsiString] text=\"" + src + "\"", LOG_INFO);
        return 0; // STATUS_SUCCESS
    }
};

// RtlFreeAnsiString(PANSI_STRING AnsiString)
class ApiRtlFreeAnsiString : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t str_addr = get_arg(cpu, 0);
        if (str_addr != 0) {
            write_u32(cpu, str_addr, 0);
            write_u64(cpu, str_addr + 8, 0);
        }
        Debug::debug_msg("[RtlFreeAnsiString] str=" + hex64(str_addr), LOG_INFO);
        return 0;
    }
};

// RtlCompareString(const STRING* String1, const STRING* String2, BOOLEAN CaseInSensitive) -> LONG
class ApiRtlCompareString : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t s1_addr = get_arg(cpu, 0);
        uint64_t s2_addr = get_arg(cpu, 1);
        uint8_t  case_in = static_cast<uint8_t>(get_arg(cpu, 2));

        std::string s1 = read_ansi_string(cpu, s1_addr);
        std::string s2 = read_ansi_string(cpu, s2_addr);

        int res = 0;
        if (case_in) {
            std::string l1 = s1, l2 = s2;
            std::transform(l1.begin(), l1.end(), l1.begin(), ::tolower);
            std::transform(l2.begin(), l2.end(), l2.begin(), ::tolower);
            res = l1.compare(l2);
        } else {
            res = s1.compare(s2);
        }

        Debug::debug_msg("[RtlCompareString] \"" + s1 + "\" vs \"" + s2 + "\" -> " + std::to_string(res), LOG_INFO);
        return static_cast<uint64_t>(res);
    }
};

// RtlEqualString(const STRING* String1, const STRING* String2, BOOLEAN CaseInSensitive) -> BOOLEAN
class ApiRtlEqualString : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t s1_addr = get_arg(cpu, 0);
        uint64_t s2_addr = get_arg(cpu, 1);
        uint8_t  case_in = static_cast<uint8_t>(get_arg(cpu, 2));

        std::string s1 = read_ansi_string(cpu, s1_addr);
        std::string s2 = read_ansi_string(cpu, s2_addr);

        bool eq = false;
        if (case_in) {
            std::string l1 = s1, l2 = s2;
            std::transform(l1.begin(), l1.end(), l1.begin(), ::tolower);
            std::transform(l2.begin(), l2.end(), l2.begin(), ::tolower);
            eq = (l1 == l2);
        } else {
            eq = (s1 == s2);
        }

        Debug::debug_msg("[RtlEqualString] \"" + s1 + "\" == \"" + s2 + "\" -> " + (eq ? "TRUE" : "FALSE"), LOG_INFO);
        return eq ? 1 : 0;
    }
};

// RtlRandom(PULONG Seed) -> ULONG
class ApiRtlRandom : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t seed_addr = get_arg(cpu, 0);
        uint32_t seed = seed_addr ? read_u32(cpu, seed_addr) : 0x12345;
        seed = (seed * 214013L + 2531011L) >> 16;
        if (seed_addr != 0) write_u32(cpu, seed_addr, seed);
        Debug::debug_msg("[RtlRandom] -> " + std::to_string(seed), LOG_INFO);
        return seed;
    }
};

// RtlRandomEx(PULONG Seed) -> ULONG
class ApiRtlRandomEx : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t seed_addr = get_arg(cpu, 0);
        uint32_t seed = seed_addr ? read_u32(cpu, seed_addr) : 0x12345;
        seed = (seed * 214013L + 2531011L);
        if (seed_addr != 0) write_u32(cpu, seed_addr, seed);
        Debug::debug_msg("[RtlRandomEx] -> " + std::to_string(seed), LOG_INFO);
        return seed;
    }
};

// RtlTimeToTimeFields(PLARGE_INTEGER Time, PTIME_FIELDS TimeFields)
class ApiRtlTimeToTimeFields : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t time_addr   = get_arg(cpu, 0);
        uint64_t fields_addr = get_arg(cpu, 1);

        if (fields_addr != 0) {
            // TIME_FIELDS layout: Year, Month, Day, Hour, Minute, Second, Milliseconds, Weekday (all CSHORT = 16-bit)
            uint16_t fields[8] = { 2026, 9, 29, 20, 0, 0, 0, 2 };
            cpu->mem_write(fields_addr, fields, sizeof(fields));
        }
        Debug::debug_msg("[RtlTimeToTimeFields]", LOG_INFO);
        return 0;
    }
};

// RtlTimeFieldsToTime(PTIME_FIELDS TimeFields, PLARGE_INTEGER Time) -> BOOLEAN
class ApiRtlTimeFieldsToTime : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t fields_addr = get_arg(cpu, 0);
        uint64_t time_addr   = get_arg(cpu, 1);

        if (time_addr != 0) {
            write_u64(cpu, time_addr, 133700000000000000ULL);
        }
        Debug::debug_msg("[RtlTimeFieldsToTime] -> TRUE", LOG_INFO);
        return 1;
    }
};
