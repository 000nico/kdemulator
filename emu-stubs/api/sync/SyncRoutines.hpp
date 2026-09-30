#pragma once
#include "../Api.hpp"
#include "../../../emu-core/src/debug/Debug.hpp"
#include "../../../sdk/hex.hpp"
#include <string>

// KeInitializeSpinLock(PKSPIN_LOCK SpinLock)
class ApiKeInitializeSpinLock : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t lock = get_arg(cpu, 0);
        write_u64(cpu, lock, 0);
        Debug::debug_msg("[KeInitializeSpinLock] lock=" + hex64(lock), LOG_INFO);
        return 0;
    }
};

// KeAcquireSpinLockRaiseToDpc(PKSPIN_LOCK SpinLock) -> KIRQL
class ApiKeAcquireSpinLockRaiseToDpc : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t lock = get_arg(cpu, 0);
        Debug::debug_msg("[KeAcquireSpinLockRaiseToDpc] lock=" + hex64(lock), LOG_INFO);
        return 0; // PASSIVE_LEVEL (old irql)
    }
};

// KeReleaseSpinLock(PKSPIN_LOCK SpinLock, KIRQL NewIrql)
class ApiKeReleaseSpinLock : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t lock     = get_arg(cpu, 0);
        uint8_t  new_irql = static_cast<uint8_t>(get_arg(cpu, 1));
        Debug::debug_msg("[KeReleaseSpinLock] lock=" + hex64(lock) + " newIrql=" + std::to_string(new_irql), LOG_INFO);
        return 0;
    }
};

// KfAcquireSpinLock(PKSPIN_LOCK SpinLock) -> KIRQL
class ApiKfAcquireSpinLock : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t lock = get_arg(cpu, 0);
        Debug::debug_msg("[KfAcquireSpinLock] lock=" + hex64(lock), LOG_INFO);
        return 0;
    }
};

// KfReleaseSpinLock(PKSPIN_LOCK SpinLock, KIRQL NewIrql)
class ApiKfReleaseSpinLock : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t lock     = get_arg(cpu, 0);
        uint8_t  new_irql = static_cast<uint8_t>(get_arg(cpu, 1));
        Debug::debug_msg("[KfReleaseSpinLock] lock=" + hex64(lock) + " newIrql=" + std::to_string(new_irql), LOG_INFO);
        return 0;
    }
};

// KeGetCurrentIrql() -> KIRQL
class ApiKeGetCurrentIrql : public Api {
public:
    uint64_t call(CPU* cpu) override {
        Debug::debug_msg("[KeGetCurrentIrql] -> PASSIVE_LEVEL(0)", LOG_INFO);
        return 0;
    }
};

// KeRaiseIrql(KIRQL NewIrql, PKIRQL OldIrql)
class ApiKeRaiseIrql : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint8_t  new_irql = static_cast<uint8_t>(get_arg(cpu, 0));
        uint64_t old_addr = get_arg(cpu, 1);
        if (old_addr != 0) {
            uint8_t old_irql = 0;
            cpu->mem_write(old_addr, &old_irql, 1);
        }
        Debug::debug_msg("[KeRaiseIrql] newIrql=" + std::to_string(new_irql), LOG_INFO);
        return 0;
    }
};

// KeLowerIrql(KIRQL NewIrql)
class ApiKeLowerIrql : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint8_t new_irql = static_cast<uint8_t>(get_arg(cpu, 0));
        Debug::debug_msg("[KeLowerIrql] newIrql=" + std::to_string(new_irql), LOG_INFO);
        return 0;
    }
};

// KeInitializeEvent(PRKEVENT Event, EVENT_TYPE Type, BOOLEAN State)
class ApiKeInitializeEvent : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t event_addr = get_arg(cpu, 0);
        uint32_t type       = static_cast<uint32_t>(get_arg(cpu, 1));
        uint8_t  state      = static_cast<uint8_t>(get_arg(cpu, 2));

        if (event_addr != 0) {
            uint32_t sign = state ? 1 : 0;
            write_u32(cpu, event_addr + 4, sign); // SignalState offset in DISPATCHER_HEADER
        }

        Debug::debug_msg("[KeInitializeEvent] event=" + hex64(event_addr) +
                         " type=" + std::to_string(type) + " state=" + std::to_string(state), LOG_INFO);
        return 0;
    }
};

// KeSetEvent(PRKEVENT Event, KPRIORITY Increment, BOOLEAN Wait) -> LONG
class ApiKeSetEvent : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t event_addr = get_arg(cpu, 0);
        uint32_t inc        = static_cast<uint32_t>(get_arg(cpu, 1));
        uint8_t  wait       = static_cast<uint8_t>(get_arg(cpu, 2));

        if (event_addr != 0) {
            write_u32(cpu, event_addr + 4, 1);
        }

        Debug::debug_msg("[KeSetEvent] event=" + hex64(event_addr) + " inc=" + std::to_string(inc), LOG_INFO);
        return 0; // Previous signal state
    }
};

// KeResetEvent(PRKEVENT Event) -> LONG
class ApiKeResetEvent : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t event_addr = get_arg(cpu, 0);
        uint32_t prev = 0;
        if (event_addr != 0) {
            prev = read_u32(cpu, event_addr + 4);
            write_u32(cpu, event_addr + 4, 0);
        }
        Debug::debug_msg("[KeResetEvent] event=" + hex64(event_addr), LOG_INFO);
        return prev;
    }
};

// KeClearEvent(PRKEVENT Event)
class ApiKeClearEvent : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t event_addr = get_arg(cpu, 0);
        if (event_addr != 0) {
            write_u32(cpu, event_addr + 4, 0);
        }
        Debug::debug_msg("[KeClearEvent] event=" + hex64(event_addr), LOG_INFO);
        return 0;
    }
};

// KeWaitForSingleObject(PVOID Object, KWAIT_REASON WaitReason, KPROCESSOR_MODE WaitMode, BOOLEAN Alertable, PLARGE_INTEGER Timeout)
class ApiKeWaitForSingleObject : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t obj     = get_arg(cpu, 0);
        uint32_t reason  = static_cast<uint32_t>(get_arg(cpu, 1));
        uint8_t  mode    = static_cast<uint8_t>(get_arg(cpu, 2));
        uint8_t  alert   = static_cast<uint8_t>(get_arg(cpu, 3));
        uint64_t timeout = get_arg(cpu, 4);

        Debug::debug_msg("[KeWaitForSingleObject] obj=" + hex64(obj) + " timeout=" + hex64(timeout), LOG_INFO);
        return 0; // STATUS_SUCCESS / STATUS_WAIT_0
    }
};

// KeWaitForMultipleObjects(...) -> NTSTATUS
class ApiKeWaitForMultipleObjects : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint32_t count = static_cast<uint32_t>(get_arg(cpu, 0));
        uint64_t objs  = get_arg(cpu, 1);
        Debug::debug_msg("[KeWaitForMultipleObjects] count=" + std::to_string(count) + " objs=" + hex64(objs), LOG_INFO);
        return 0; // STATUS_WAIT_0
    }
};

// KeInitializeMutex(PRKMUTEX Mutex, ULONG Level)
class ApiKeInitializeMutex : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t mutex = get_arg(cpu, 0);
        uint32_t level = static_cast<uint32_t>(get_arg(cpu, 1));
        Debug::debug_msg("[KeInitializeMutex] mutex=" + hex64(mutex) + " level=" + std::to_string(level), LOG_INFO);
        return 0;
    }
};

// KeReleaseMutex(PRKMUTEX Mutex, BOOLEAN Wait) -> LONG
class ApiKeReleaseMutex : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t mutex = get_arg(cpu, 0);
        Debug::debug_msg("[KeReleaseMutex] mutex=" + hex64(mutex), LOG_INFO);
        return 0;
    }
};

// KeInitializeSemaphore(PRKSEMAPHORE Semaphore, LONG Count, LONG Limit)
class ApiKeInitializeSemaphore : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t sem   = get_arg(cpu, 0);
        int32_t  count = static_cast<int32_t>(get_arg(cpu, 1));
        int32_t  limit = static_cast<int32_t>(get_arg(cpu, 2));
        Debug::debug_msg("[KeInitializeSemaphore] sem=" + hex64(sem) +
                         " count=" + std::to_string(count) + " limit=" + std::to_string(limit), LOG_INFO);
        return 0;
    }
};

// KeReleaseSemaphore(...) -> LONG
class ApiKeReleaseSemaphore : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t sem = get_arg(cpu, 0);
        Debug::debug_msg("[KeReleaseSemaphore] sem=" + hex64(sem), LOG_INFO);
        return 0;
    }
};

// ExInitializeFastMutex(PFAST_MUTEX FastMutex)
class ApiExInitializeFastMutex : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t mtx = get_arg(cpu, 0);
        write_u32(cpu, mtx, 1); // Count = 1
        Debug::debug_msg("[ExInitializeFastMutex] mtx=" + hex64(mtx), LOG_INFO);
        return 0;
    }
};

// ExAcquireFastMutex(PFAST_MUTEX FastMutex)
class ApiExAcquireFastMutex : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t mtx = get_arg(cpu, 0);
        Debug::debug_msg("[ExAcquireFastMutex] mtx=" + hex64(mtx), LOG_INFO);
        return 0;
    }
};

// ExReleaseFastMutex(PFAST_MUTEX FastMutex)
class ApiExReleaseFastMutex : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t mtx = get_arg(cpu, 0);
        Debug::debug_msg("[ExReleaseFastMutex] mtx=" + hex64(mtx), LOG_INFO);
        return 0;
    }
};

// ExInitializePushLock(PULONG_PTR PushLock)
class ApiExInitializePushLock : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t lock = get_arg(cpu, 0);
        write_u64(cpu, lock, 0);
        Debug::debug_msg("[ExInitializePushLock] lock=" + hex64(lock), LOG_INFO);
        return 0;
    }
};

// ExAcquirePushLockExclusive(PULONG_PTR PushLock)
class ApiExAcquirePushLockExclusive : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t lock = get_arg(cpu, 0);
        Debug::debug_msg("[ExAcquirePushLockExclusive] lock=" + hex64(lock), LOG_INFO);
        return 0;
    }
};

// ExReleasePushLockExclusive(PULONG_PTR PushLock)
class ApiExReleasePushLockExclusive : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t lock = get_arg(cpu, 0);
        Debug::debug_msg("[ExReleasePushLockExclusive] lock=" + hex64(lock), LOG_INFO);
        return 0;
    }
};

// ExAcquirePushLockShared(PULONG_PTR PushLock)
class ApiExAcquirePushLockShared : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t lock = get_arg(cpu, 0);
        Debug::debug_msg("[ExAcquirePushLockShared] lock=" + hex64(lock), LOG_INFO);
        return 0;
    }
};

// ExReleasePushLockShared(PULONG_PTR PushLock)
class ApiExReleasePushLockShared : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t lock = get_arg(cpu, 0);
        Debug::debug_msg("[ExReleasePushLockShared] lock=" + hex64(lock), LOG_INFO);
        return 0;
    }
};

// KeDelayExecutionThread(KPROCESSOR_MODE WaitMode, BOOLEAN Alertable, PLARGE_INTEGER Interval)
class ApiKeDelayExecutionThread : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t interval_addr = get_arg(cpu, 2);
        int64_t interval = static_cast<int64_t>(read_u64(cpu, interval_addr));
        Debug::debug_msg("[KeDelayExecutionThread] interval=" + std::to_string(interval), LOG_INFO);
        return 0; // STATUS_SUCCESS
    }
};

// KeQuerySystemTime(PLARGE_INTEGER CurrentTime)
class ApiKeQuerySystemTime : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t out_time = get_arg(cpu, 0);
        uint64_t time_val = 133700000000000000ULL; // Standard simulated Windows 100-ns timestamp
        if (out_time != 0) {
            write_u64(cpu, out_time, time_val);
        }
        Debug::debug_msg("[KeQuerySystemTime] -> " + hex64(time_val), LOG_INFO);
        return 0;
    }
};

// KeQuerySystemTimePrecise(PLARGE_INTEGER CurrentTime)
class ApiKeQuerySystemTimePrecise : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t out_time = get_arg(cpu, 0);
        uint64_t time_val = 133700000000000000ULL;
        if (out_time != 0) {
            write_u64(cpu, out_time, time_val);
        }
        Debug::debug_msg("[KeQuerySystemTimePrecise] -> " + hex64(time_val), LOG_INFO);
        return 0;
    }
};

// KeQueryPerformanceCounter(PLARGE_INTEGER PerformanceFreq) -> LARGE_INTEGER
class ApiKeQueryPerformanceCounter : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t freq_addr = get_arg(cpu, 0);
        if (freq_addr != 0) {
            write_u64(cpu, freq_addr, 10000000ULL); // 10 MHz standard QPC frequency
        }
        static uint64_t s_qpc = 1000000ULL;
        s_qpc += 1000;
        Debug::debug_msg("[KeQueryPerformanceCounter] -> " + std::to_string(s_qpc), LOG_INFO);
        return s_qpc;
    }
};

// KeQueryTickCount(PLARGE_INTEGER TickCount)
class ApiKeQueryTickCount : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t tick_addr = get_arg(cpu, 0);
        static uint64_t s_tick = 5000ULL;
        s_tick += 10;
        if (tick_addr != 0) {
            write_u64(cpu, tick_addr, s_tick);
        }
        Debug::debug_msg("[KeQueryTickCount] -> " + std::to_string(s_tick), LOG_INFO);
        return s_tick;
    }
};

// KeQueryTimeIncrement() -> ULONG
class ApiKeQueryTimeIncrement : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint32_t inc = 100000; // 10ms in 100ns units
        Debug::debug_msg("[KeQueryTimeIncrement] -> " + std::to_string(inc), LOG_INFO);
        return inc;
    }
};

// KeInitializeTimer(PKTIMER Timer)
class ApiKeInitializeTimer : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t timer = get_arg(cpu, 0);
        Debug::debug_msg("[KeInitializeTimer] timer=" + hex64(timer), LOG_INFO);
        return 0;
    }
};

// KeInitializeTimerEx(PKTIMER Timer, TIMER_TYPE Type)
class ApiKeInitializeTimerEx : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t timer = get_arg(cpu, 0);
        uint32_t type  = static_cast<uint32_t>(get_arg(cpu, 1));
        Debug::debug_msg("[KeInitializeTimerEx] timer=" + hex64(timer) + " type=" + std::to_string(type), LOG_INFO);
        return 0;
    }
};

// KeSetTimer(PKTIMER Timer, LARGE_INTEGER DueTime, PKDPC Dpc) -> BOOLEAN
class ApiKeSetTimer : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t timer = get_arg(cpu, 0);
        uint64_t due   = get_arg(cpu, 1);
        uint64_t dpc   = get_arg(cpu, 2);
        Debug::debug_msg("[KeSetTimer] timer=" + hex64(timer) + " due=" + hex64(due) + " dpc=" + hex64(dpc), LOG_INFO);
        return 0;
    }
};

// KeSetTimerEx(PKTIMER Timer, LARGE_INTEGER DueTime, LONG Period, PKDPC Dpc) -> BOOLEAN
class ApiKeSetTimerEx : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t timer  = get_arg(cpu, 0);
        uint64_t due    = get_arg(cpu, 1);
        int32_t  period = static_cast<int32_t>(get_arg(cpu, 2));
        uint64_t dpc    = get_arg(cpu, 3);
        Debug::debug_msg("[KeSetTimerEx] timer=" + hex64(timer) + " period=" + std::to_string(period), LOG_INFO);
        return 0;
    }
};

// KeCancelTimer(PKTIMER Timer) -> BOOLEAN
class ApiKeCancelTimer : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t timer = get_arg(cpu, 0);
        Debug::debug_msg("[KeCancelTimer] timer=" + hex64(timer), LOG_INFO);
        return 1;
    }
};

// KeInitializeDpc(PRKDPC Dpc, PKDEFERRED_ROUTINE DeferredRoutine, PVOID DeferredContext)
class ApiKeInitializeDpc : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t dpc     = get_arg(cpu, 0);
        uint64_t routine = get_arg(cpu, 1);
        uint64_t context = get_arg(cpu, 2);
        Debug::debug_msg("[KeInitializeDpc] dpc=" + hex64(dpc) + " routine=" + hex64(routine), LOG_INFO);
        return 0;
    }
};

// KeInsertQueueDpc(PRKDPC Dpc, PVOID SystemArgument1, PVOID SystemArgument2) -> BOOLEAN
class ApiKeInsertQueueDpc : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t dpc = get_arg(cpu, 0);
        Debug::debug_msg("[KeInsertQueueDpc] dpc=" + hex64(dpc), LOG_INFO);
        return 1;
    }
};

// KeRemoveQueueDpc(PRKDPC Dpc) -> BOOLEAN
class ApiKeRemoveQueueDpc : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t dpc = get_arg(cpu, 0);
        Debug::debug_msg("[KeRemoveQueueDpc] dpc=" + hex64(dpc), LOG_INFO);
        return 1;
    }
};

// KeQueryActiveProcessors() -> KAFFINITY
class ApiKeQueryActiveProcessors : public Api {
public:
    uint64_t call(CPU* cpu) override {
        Debug::debug_msg("[KeQueryActiveProcessors] -> 0x0F", LOG_INFO);
        return 0x0FULL; // 4 active processors
    }
};

// KeQueryActiveProcessorCount(PKAFFINITY ActiveProcessors) -> ULONG
class ApiKeQueryActiveProcessorCount : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t aff_addr = get_arg(cpu, 0);
        if (aff_addr != 0) {
            write_u64(cpu, aff_addr, 0x0FULL);
        }
        Debug::debug_msg("[KeQueryActiveProcessorCount] -> 4", LOG_INFO);
        return 4;
    }
};

// KeQueryActiveProcessorCountEx(USHORT GroupNumber) -> ULONG
class ApiKeQueryActiveProcessorCountEx : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint16_t group = static_cast<uint16_t>(get_arg(cpu, 0));
        Debug::debug_msg("[KeQueryActiveProcessorCountEx] group=" + std::to_string(group) + " -> 4", LOG_INFO);
        return 4;
    }
};
