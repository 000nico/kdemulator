#pragma once
#include "../Api.hpp"
#include "../memory/allocators_common.hpp"
#include "../../../emu-core/src/debug/Debug.hpp"
#include "../../../sdk/hex.hpp"
#include <string>

// FltRegisterFilter(PDRIVER_OBJECT Driver, const FLT_REGISTRATION* Registration, PFLT_FILTER* RetFilter) -> NTSTATUS
class ApiFltRegisterFilter : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t drv     = get_arg(cpu, 0);
        uint64_t reg     = get_arg(cpu, 1);
        uint64_t out_flt = get_arg(cpu, 2);

        uint64_t flt = 0xFFFFFA8000005000ULL;
        if (out_flt != 0) {
            write_u64(cpu, out_flt, flt);
        }

        Debug::debug_msg("[FltRegisterFilter] drv=" + hex64(drv) + " reg=" + hex64(reg) + " -> filter=" + hex64(flt), LOG_INFO);
        return 0; // STATUS_SUCCESS
    }
};

// FltUnregisterFilter(PFLT_FILTER Filter)
class ApiFltUnregisterFilter : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t flt = get_arg(cpu, 0);
        Debug::debug_msg("[FltUnregisterFilter] filter=" + hex64(flt), LOG_INFO);
        return 0;
    }
};

// FltStartFiltering(PFLT_FILTER Filter) -> NTSTATUS
class ApiFltStartFiltering : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t flt = get_arg(cpu, 0);
        Debug::debug_msg("[FltStartFiltering] filter=" + hex64(flt), LOG_INFO);
        return 0; // STATUS_SUCCESS
    }
};

// FltGetFileNameInformation(PFLT_CALLBACK_DATA CallbackData, FLT_FILE_NAME_OPTIONS NameOptions, PFLT_FILE_NAME_INFORMATION* FileNameInformation)
class ApiFltGetFileNameInformation : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t cb_data = get_arg(cpu, 0);
        uint32_t options = static_cast<uint32_t>(get_arg(cpu, 1));
        uint64_t out_info = get_arg(cpu, 2);

        static uint64_t s_info = 0;
        if (s_info == 0) {
            s_info = allocate(0x80, 0, 0x46746C46, cpu); // 'FltF'
        }

        if (out_info != 0) {
            write_u64(cpu, out_info, s_info);
        }

        Debug::debug_msg("[FltGetFileNameInformation] cbData=" + hex64(cb_data) + " -> info=" + hex64(s_info), LOG_INFO);
        return 0; // STATUS_SUCCESS
    }
};

// FltReleaseFileNameInformation(PFLT_FILE_NAME_INFORMATION FileNameInformation)
class ApiFltReleaseFileNameInformation : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t info = get_arg(cpu, 0);
        Debug::debug_msg("[FltReleaseFileNameInformation] info=" + hex64(info), LOG_INFO);
        return 0;
    }
};

// FltParseFileNameInformation(PFLT_FILE_NAME_INFORMATION FileNameInformation) -> NTSTATUS
class ApiFltParseFileNameInformation : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t info = get_arg(cpu, 0);
        Debug::debug_msg("[FltParseFileNameInformation] info=" + hex64(info), LOG_INFO);
        return 0; // STATUS_SUCCESS
    }
};
