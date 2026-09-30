#pragma once
#include "../Api.hpp"
#include "../../../emu-core/src/debug/Debug.hpp"
#include "../../../sdk/hex.hpp"
#include <string>

// PsSetCreateProcessNotifyRoutine(PCREATE_PROCESS_NOTIFY_ROUTINE NotifyRoutine, BOOLEAN Remove)
class ApiPsSetCreateProcessNotifyRoutine : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t routine = get_arg(cpu, 0);
        uint8_t remove   = static_cast<uint8_t>(get_arg(cpu, 1));
        Debug::debug_msg("[PsSetCreateProcessNotifyRoutine] routine=" + hex64(routine) +
                         " remove=" + std::to_string(remove), LOG_INFO);
        return 0; // STATUS_SUCCESS
    }
};

// PsSetCreateProcessNotifyRoutineEx(PCREATE_PROCESS_NOTIFY_ROUTINE_EX NotifyRoutine, BOOLEAN Remove)
class ApiPsSetCreateProcessNotifyRoutineEx : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t routine = get_arg(cpu, 0);
        uint8_t remove   = static_cast<uint8_t>(get_arg(cpu, 1));
        Debug::debug_msg("[PsSetCreateProcessNotifyRoutineEx] routine=" + hex64(routine) +
                         " remove=" + std::to_string(remove), LOG_INFO);
        return 0; // STATUS_SUCCESS
    }
};

// PsSetCreateProcessNotifyRoutineEx2(PS_CREATE_PROCESS_NOTIFY_TYPE NotifyType, PVOID NotifyInformation, BOOLEAN Remove)
class ApiPsSetCreateProcessNotifyRoutineEx2 : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint32_t type   = static_cast<uint32_t>(get_arg(cpu, 0));
        uint64_t info   = get_arg(cpu, 1);
        uint8_t remove  = static_cast<uint8_t>(get_arg(cpu, 2));
        Debug::debug_msg("[PsSetCreateProcessNotifyRoutineEx2] type=" + std::to_string(type) +
                         " info=" + hex64(info) + " remove=" + std::to_string(remove), LOG_INFO);
        return 0; // STATUS_SUCCESS
    }
};

// PsSetCreateThreadNotifyRoutine(PCREATE_THREAD_NOTIFY_ROUTINE NotifyRoutine)
class ApiPsSetCreateThreadNotifyRoutine : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t routine = get_arg(cpu, 0);
        Debug::debug_msg("[PsSetCreateThreadNotifyRoutine] routine=" + hex64(routine), LOG_INFO);
        return 0; // STATUS_SUCCESS
    }
};

// PsSetCreateThreadNotifyRoutineEx(PS_CREATE_THREAD_NOTIFY_TYPE NotifyType, PVOID NotifyInformation)
class ApiPsSetCreateThreadNotifyRoutineEx : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint32_t type = static_cast<uint32_t>(get_arg(cpu, 0));
        uint64_t info = get_arg(cpu, 1);
        Debug::debug_msg("[PsSetCreateThreadNotifyRoutineEx] type=" + std::to_string(type) +
                         " info=" + hex64(info), LOG_INFO);
        return 0; // STATUS_SUCCESS
    }
};

// PsRemoveCreateThreadNotifyRoutine(PCREATE_THREAD_NOTIFY_ROUTINE NotifyRoutine)
class ApiPsRemoveCreateThreadNotifyRoutine : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t routine = get_arg(cpu, 0);
        Debug::debug_msg("[PsRemoveCreateThreadNotifyRoutine] routine=" + hex64(routine), LOG_INFO);
        return 0; // STATUS_SUCCESS
    }
};

// PsSetLoadImageNotifyRoutine(PLOAD_IMAGE_NOTIFY_ROUTINE NotifyRoutine)
class ApiPsSetLoadImageNotifyRoutine : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t routine = get_arg(cpu, 0);
        Debug::debug_msg("[PsSetLoadImageNotifyRoutine] routine=" + hex64(routine), LOG_INFO);
        return 0; // STATUS_SUCCESS
    }
};

// PsRemoveLoadImageNotifyRoutine(PLOAD_IMAGE_NOTIFY_ROUTINE NotifyRoutine)
class ApiPsRemoveLoadImageNotifyRoutine : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t routine = get_arg(cpu, 0);
        Debug::debug_msg("[PsRemoveLoadImageNotifyRoutine] routine=" + hex64(routine), LOG_INFO);
        return 0; // STATUS_SUCCESS
    }
};

// PsSetLoadImageNotifyRoutineEx(PLOAD_IMAGE_NOTIFY_ROUTINE NotifyRoutine, ULONG_PTR Flags) -> NTSTATUS
class ApiPsSetLoadImageNotifyRoutineEx : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t routine = get_arg(cpu, 0);
        uint64_t flags   = get_arg(cpu, 1);
        Debug::debug_msg("[PsSetLoadImageNotifyRoutineEx] routine=" + hex64(routine) + " flags=" + hex64(flags), LOG_INFO);
        return 0; // STATUS_SUCCESS
    }
};

// ObRegisterCallbacks(POB_CALLBACK_REGISTRATION CallbackRegistration, PVOID* RegistrationHandle)
class ApiObRegisterCallbacks : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t reg_addr    = get_arg(cpu, 0);
        uint64_t handle_addr = get_arg(cpu, 1);

        static uint64_t s_ob_handle_counter = 0xFFFFFA8000CB0001ULL;
        uint64_t reg_handle = s_ob_handle_counter++;

        if (handle_addr != 0) {
            write_u64(cpu, handle_addr, reg_handle);
        }

        Debug::debug_msg("[ObRegisterCallbacks] reg=" + hex64(reg_addr) +
                         " -> handle=" + hex64(reg_handle), LOG_INFO);
        return 0; // STATUS_SUCCESS
    }
};

// ObUnRegisterCallbacks(PVOID RegistrationHandle)
class ApiObUnRegisterCallbacks : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t handle = get_arg(cpu, 0);
        Debug::debug_msg("[ObUnRegisterCallbacks] handle=" + hex64(handle), LOG_INFO);
        return 0;
    }
};

// CmRegisterCallback(PEX_CALLBACK_FUNCTION Function, PVOID Context, PLARGE_INTEGER Cookie)
class ApiCmRegisterCallback : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t fn          = get_arg(cpu, 0);
        uint64_t context     = get_arg(cpu, 1);
        uint64_t cookie_addr = get_arg(cpu, 2);

        static uint64_t s_cm_cookie = 0x1000;
        uint64_t cookie_val = s_cm_cookie++;

        if (cookie_addr != 0) {
            write_u64(cpu, cookie_addr, cookie_val);
        }

        Debug::debug_msg("[CmRegisterCallback] fn=" + hex64(fn) +
                         " context=" + hex64(context) + " cookie=" + hex64(cookie_val), LOG_INFO);
        return 0; // STATUS_SUCCESS
    }
};

// CmRegisterCallbackEx(PEX_CALLBACK_FUNCTION Function, PCUNICODE_STRING Altitude, PVOID Driver, PVOID Context, PLARGE_INTEGER Cookie, PVOID Reserved)
class ApiCmRegisterCallbackEx : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t fn          = get_arg(cpu, 0);
        uint64_t altitude    = get_arg(cpu, 1);
        uint64_t cookie_addr = get_arg(cpu, 4);

        std::string alt_str = read_unicode_string(cpu, altitude);
        static uint64_t s_cm_cookie_ex = 0x2000;
        uint64_t cookie_val = s_cm_cookie_ex++;

        if (cookie_addr != 0) {
            write_u64(cpu, cookie_addr, cookie_val);
        }

        Debug::debug_msg("[CmRegisterCallbackEx] fn=" + hex64(fn) +
                         " alt=\"" + alt_str + "\" cookie=" + hex64(cookie_val), LOG_INFO);
        return 0; // STATUS_SUCCESS
    }
};

// CmUnRegisterCallback(LARGE_INTEGER Cookie)
class ApiCmUnRegisterCallback : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t cookie = get_arg(cpu, 0);
        Debug::debug_msg("[CmUnRegisterCallback] cookie=" + hex64(cookie), LOG_INFO);
        return 0; // STATUS_SUCCESS
    }
};
