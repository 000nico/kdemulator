#pragma once
#include "../Api.hpp"
#include "../memory/allocators_common.hpp"
#include "../../../emu-core/src/debug/Debug.hpp"
#include "../../../sdk/hex.hpp"
#include <string>

inline uint64_t get_dummy_eprocess(CPU* cpu) {
    static uint64_t s_eprocess = 0;
    if (s_eprocess == 0) {
        s_eprocess = allocate(0x1000, 0, 0x636F7250, cpu); // 'Proc'
        char name[16] = "System";
        cpu->mem_write(s_eprocess + 0x2E0, name, sizeof(name));
        cpu->mem_write(s_eprocess + 0x450, name, sizeof(name));
        cpu->mem_write(s_eprocess + 0x5A8, name, sizeof(name));

        uint64_t pid = 4;
        cpu->mem_write(s_eprocess + 0x2D8, &pid, sizeof(pid));
        cpu->mem_write(s_eprocess + 0x448, &pid, sizeof(pid));

        uint64_t base = 0x00007FF700000000ULL;
        cpu->mem_write(s_eprocess + 0x3F8, &base, sizeof(base));
        cpu->mem_write(s_eprocess + 0x520, &base, sizeof(base));
    }
    return s_eprocess;
}

inline uint64_t get_dummy_ethread(CPU* cpu) {
    static uint64_t s_ethread = 0;
    if (s_ethread == 0) {
        s_ethread = allocate(0x1000, 0, 0x64726854, cpu); // 'Thrd'
        uint64_t tid = 0x1004;
        cpu->mem_write(s_ethread + 0x480, &tid, sizeof(tid));
        cpu->mem_write(s_ethread + 0x648, &tid, sizeof(tid));
    }
    return s_ethread;
}

// PsGetCurrentProcess() -> PEPROCESS
class ApiPsGetCurrentProcess : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t proc = get_dummy_eprocess(cpu);
        Debug::debug_msg("[PsGetCurrentProcess] -> " + hex64(proc), LOG_INFO);
        return proc;
    }
};

// PsGetCurrentProcessId() -> HANDLE
class ApiPsGetCurrentProcessId : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t pid = 4; // System PID
        Debug::debug_msg("[PsGetCurrentProcessId] -> " + std::to_string(pid), LOG_INFO);
        return pid;
    }
};

// PsGetCurrentThread() -> PETHREAD
class ApiPsGetCurrentThread : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t thrd = get_dummy_ethread(cpu);
        Debug::debug_msg("[PsGetCurrentThread] -> " + hex64(thrd), LOG_INFO);
        return thrd;
    }
};

// PsGetCurrentThreadId() -> HANDLE
class ApiPsGetCurrentThreadId : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t tid = 0x1004;
        Debug::debug_msg("[PsGetCurrentThreadId] -> " + hex64(tid), LOG_INFO);
        return tid;
    }
};

// PsLookupProcessByProcessId(HANDLE ProcessId, PEPROCESS* Process) -> NTSTATUS
class ApiPsLookupProcessByProcessId : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t pid        = get_arg(cpu, 0);
        uint64_t out_proc   = get_arg(cpu, 1);
        uint64_t proc_addr  = get_dummy_eprocess(cpu);

        if (out_proc != 0) {
            write_u64(cpu, out_proc, proc_addr);
        }

        Debug::debug_msg("[PsLookupProcessByProcessId] pid=" + std::to_string(pid) +
                         " -> proc=" + hex64(proc_addr), LOG_INFO);
        return 0; // STATUS_SUCCESS
    }
};

// PsLookupThreadByThreadId(HANDLE ThreadId, PETHREAD* Thread) -> NTSTATUS
class ApiPsLookupThreadByThreadId : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t tid        = get_arg(cpu, 0);
        uint64_t out_thread = get_arg(cpu, 1);
        uint64_t thrd_addr  = get_dummy_ethread(cpu);

        if (out_thread != 0) {
            write_u64(cpu, out_thread, thrd_addr);
        }

        Debug::debug_msg("[PsLookupThreadByThreadId] tid=" + hex64(tid) +
                         " -> thrd=" + hex64(thrd_addr), LOG_INFO);
        return 0; // STATUS_SUCCESS
    }
};

// PsGetProcessId(PEPROCESS Process) -> HANDLE
class ApiPsGetProcessId : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t proc = get_arg(cpu, 0);
        uint64_t pid  = 4;
        Debug::debug_msg("[PsGetProcessId] proc=" + hex64(proc) + " -> pid=" + std::to_string(pid), LOG_INFO);
        return pid;
    }
};

// PsGetThreadId(PETHREAD Thread) -> HANDLE
class ApiPsGetThreadId : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t thrd = get_arg(cpu, 0);
        uint64_t tid  = 0x1004;
        Debug::debug_msg("[PsGetThreadId] thrd=" + hex64(thrd) + " -> tid=" + hex64(tid), LOG_INFO);
        return tid;
    }
};

// PsGetThreadProcessId(PETHREAD Thread) -> HANDLE
class ApiPsGetThreadProcessId : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t thrd = get_arg(cpu, 0);
        uint64_t pid  = 4;
        Debug::debug_msg("[PsGetThreadProcessId] thrd=" + hex64(thrd) + " -> pid=" + std::to_string(pid), LOG_INFO);
        return pid;
    }
};

// PsGetProcessImageFileName(PEPROCESS Process) -> PCHAR
class ApiPsGetProcessImageFileName : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t proc = get_arg(cpu, 0);
        if (proc == 0) proc = get_dummy_eprocess(cpu);
        uint64_t str_addr = proc + 0x5A8; // ImageFileName offset in dummy struct
        Debug::debug_msg("[PsGetProcessImageFileName] proc=" + hex64(proc) + " -> \"System\"", LOG_INFO);
        return str_addr;
    }
};

// PsGetProcessSectionBaseAddress(PEPROCESS Process) -> PVOID
class ApiPsGetProcessSectionBaseAddress : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t proc = get_arg(cpu, 0);
        uint64_t base = 0x00007FF700000000ULL;
        Debug::debug_msg("[PsGetProcessSectionBaseAddress] proc=" + hex64(proc) + " -> " + hex64(base), LOG_INFO);
        return base;
    }
};

// PsGetProcessPeb(PEPROCESS Process) -> PPEB
class ApiPsGetProcessPeb : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t proc = get_arg(cpu, 0);
        uint64_t peb  = 0x00007FFE0000ULL;
        Debug::debug_msg("[PsGetProcessPeb] proc=" + hex64(proc) + " -> " + hex64(peb), LOG_INFO);
        return peb;
    }
};

// PsIsProtectedProcess(PEPROCESS Process) -> BOOLEAN
class ApiPsIsProtectedProcess : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t proc = get_arg(cpu, 0);
        Debug::debug_msg("[PsIsProtectedProcess] proc=" + hex64(proc) + " -> FALSE", LOG_INFO);
        return 0;
    }
};

// PsIsSystemProcess(PEPROCESS Process) -> BOOLEAN
class ApiPsIsSystemProcess : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t proc = get_arg(cpu, 0);
        Debug::debug_msg("[PsIsSystemProcess] proc=" + hex64(proc) + " -> TRUE", LOG_INFO);
        return 1;
    }
};

// IoGetCurrentProcess() -> PEPROCESS
class ApiIoGetCurrentProcess : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t proc = get_dummy_eprocess(cpu);
        Debug::debug_msg("[IoGetCurrentProcess] -> " + hex64(proc), LOG_INFO);
        return proc;
    }
};

// IoThreadToProcess(PETHREAD Thread) -> PEPROCESS
class ApiIoThreadToProcess : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t thrd = get_arg(cpu, 0);
        uint64_t proc = get_dummy_eprocess(cpu);
        Debug::debug_msg("[IoThreadToProcess] thrd=" + hex64(thrd) + " -> proc=" + hex64(proc), LOG_INFO);
        return proc;
    }
};

// PsCreateSystemThread(PHANDLE ThreadHandle, ULONG DesiredAccess, POBJECT_ATTRIBUTES ObjectAttributes, HANDLE ProcessHandle, PCLIENT_ID ClientId, PKSTART_ROUTINE StartRoutine, PVOID StartContext)
class ApiPsCreateSystemThread : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t handle_addr = get_arg(cpu, 0);
        uint64_t routine     = get_arg(cpu, 5);
        if (handle_addr != 0) {
            write_u64(cpu, handle_addr, 0x108);
        }
        Debug::debug_msg("[PsCreateSystemThread] startRoutine=" + hex64(routine) + " -> handle=0x108", LOG_INFO);
        return 0; // STATUS_SUCCESS
    }
};

// PsTerminateSystemThread(NTSTATUS ExitStatus)
class ApiPsTerminateSystemThread : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint32_t status = static_cast<uint32_t>(get_arg(cpu, 0));
        Debug::debug_msg("[PsTerminateSystemThread] status=" + hex32(status), LOG_INFO);
        return 0;
    }
};
