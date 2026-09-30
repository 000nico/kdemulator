#pragma once
#include "../Api.hpp"
#include "../../../emu-core/src/debug/Debug.hpp"
#include "../../../sdk/hex.hpp"
#include <string>

// EtwRegister(LPCGUID ProviderId, PETWENABLECALLBACK EnableCallback, PVOID CallbackContext, PREGHANDLE RegHandle) -> NTSTATUS
class ApiEtwRegister : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t prov_id  = get_arg(cpu, 0);
        uint64_t cb       = get_arg(cpu, 1);
        uint64_t ctx      = get_arg(cpu, 2);
        uint64_t out_reg  = get_arg(cpu, 3);

        uint64_t handle = 0x500;
        if (out_reg != 0) {
            write_u64(cpu, out_reg, handle);
        }

        Debug::debug_msg("[EtwRegister] prov=" + hex64(prov_id) + " -> handle=" + hex64(handle), LOG_INFO);
        return 0; // STATUS_SUCCESS
    }
};

// EtwUnregister(REGHANDLE RegHandle) -> NTSTATUS
class ApiEtwUnregister : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t handle = get_arg(cpu, 0);
        Debug::debug_msg("[EtwUnregister] handle=" + hex64(handle), LOG_INFO);
        return 0; // STATUS_SUCCESS
    }
};

// EtwWrite(REGHANDLE RegHandle, PCEVENT_DESCRIPTOR EventDescriptor, LPCGUID ActivityId, ULONG UserDataCount, PEVENT_DATA_DESCRIPTOR UserData)
class ApiEtwWrite : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t handle = get_arg(cpu, 0);
        uint32_t count  = static_cast<uint32_t>(get_arg(cpu, 3));
        Debug::debug_msg("[EtwWrite] handle=" + hex64(handle) + " count=" + std::to_string(count), LOG_INFO);
        return 0; // STATUS_SUCCESS
    }
};

// HalGetBusData(...) -> ULONG
class ApiHalGetBusData : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint32_t bus_type = static_cast<uint32_t>(get_arg(cpu, 0));
        uint32_t bus_num  = static_cast<uint32_t>(get_arg(cpu, 1));
        uint32_t slot     = static_cast<uint32_t>(get_arg(cpu, 2));
        Debug::debug_msg("[HalGetBusData] type=" + std::to_string(bus_type) + " bus=" + std::to_string(bus_num), LOG_INFO);
        return 0;
    }
};

// HalSetBusData(...) -> ULONG
class ApiHalSetBusData : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint32_t bus_type = static_cast<uint32_t>(get_arg(cpu, 0));
        uint32_t bus_num  = static_cast<uint32_t>(get_arg(cpu, 1));
        uint32_t slot     = static_cast<uint32_t>(get_arg(cpu, 2));
        Debug::debug_msg("[HalSetBusData] type=" + std::to_string(bus_type) + " bus=" + std::to_string(bus_num), LOG_INFO);
        return 0;
    }
};
