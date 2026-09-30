#include "APIDispatcher.hpp"
#include "../api/ob/IoCreateDevice.hpp"
#include "../emu-core/src/memory/layout.hpp"
#include "../api/out/DbgPrint.hpp"
#include "../api/out/KdPrint.hpp"
#include "../api/out/DbgPrintEx.hpp"
#include "../api/memory/ExAllocatePool.hpp"
#include "../api/memory/ExAllocatePool2.hpp"
#include "../api/memory/ExAllocatePool3.hpp"
#include "../api/memory/ExAllocatePoolWithTag.hpp"
#include "../api/memory/ExFreePoolWithTag.hpp"
#include "../api/memory/ExFreePool.hpp"
#include "../api/rtl/RtlInitUnicodeString.hpp"
#include "../api/rtl/RtlZeroMemory.hpp"
#include "../api/rtl/RtlCopyMemory.hpp"
#include "../api/rtl/RtlCopyUnicodeString.hpp"
#include "../api/rtl/RtlUnicodeStringToInteger.hpp"
#include "../api/rtl/RtlIntegerToUnicodeString.hpp"
#include "../api/rtl/RtlAppendUnicodeStringToString.hpp"

// New modules
#include "../api/callbacks/Callbacks.hpp"
#include "../api/ps/ProcessThread.hpp"
#include "../api/ob/ObRoutines.hpp"
#include "../api/memory/MmRoutines.hpp"
#include "../api/sync/SyncRoutines.hpp"
#include "../api/zw/ZwRoutines.hpp"
#include "../api/rtl/RtlExtended.hpp"
#include "../api/flt/Minifilter.hpp"
#include "../api/etw/EtwRoutines.hpp"
#include "../api/cng/BCrypt.hpp"

APIDispatcher::APIDispatcher(){
    // Dbg / Out
    register_api("DbgPrint", new ApiDbgPrint());
    register_api("KdPrint",  new ApiKdPrint());
    register_api("DbgPrintEx", new ApiDbgPrintEx());

    // Pool Memory
    register_api("ExAllocatePool", new ApiExAllocatePool());
    register_api("ExAllocatePool2", new ApiExAllocatePool2());
    register_api("ExAllocatePool3", new ApiExAllocatePool3());
    auto* poolWithTag = new ApiExAllocatePoolWithTag();
    register_api("ExAllocatePoolWithTag", poolWithTag);
    register_api("ExAllocatePoolWithQuota", poolWithTag);
    register_api("ExAllocatePoolWithQuotaTag", poolWithTag);
    register_api("ExFreePool", new ApiExFreePool());
    register_api("ExFreePoolWithTag", new ApiExFreePoolWithTag());

    // RTL Basic
    register_api("RtlZeroMemory", new ApiRtlZeroMemory());
    register_api("RtlCopyMemory", new ApiRtlCopyMemory());
    register_api("RtlCopyUnicodeString", new ApiRtlCopyUnicodeString());
    register_api("RtlInitUnicodeString", new ApiRtlInitUnicodeString());
    register_api("RtlUnicodeStringToInteger", new ApiRtlUnicodeStringToInteger());
    register_api("RtlIntegerToUnicodeString", new ApiRtlIntegerToUnicodeString());
    register_api("RtlAppendUnicodeStringToString", new ApiRtlAppendUnicodeStringToString());

    // RTL Extended
    register_api("RtlGetVersion", new ApiRtlGetVersion());
    register_api("RtlCompareMemory", new ApiRtlCompareMemory());
    register_api("RtlEqualMemory", new ApiRtlEqualMemory());
    register_api("RtlFillMemory", new ApiRtlFillMemory());
    register_api("RtlMoveMemory", new ApiRtlMoveMemory());
    register_api("RtlCompareUnicodeString", new ApiRtlCompareUnicodeString());
    register_api("RtlEqualUnicodeString", new ApiRtlEqualUnicodeString());
    register_api("RtlUpcaseUnicodeString", new ApiRtlUpcaseUnicodeString());
    register_api("RtlFreeUnicodeString", new ApiRtlFreeUnicodeString());
    register_api("RtlInitAnsiString", new ApiRtlInitAnsiString());
    register_api("RtlAnsiStringToUnicodeString", new ApiRtlAnsiStringToUnicodeString());
    register_api("RtlUnicodeStringToAnsiString", new ApiRtlUnicodeStringToAnsiString());
    register_api("RtlFreeAnsiString", new ApiRtlFreeAnsiString());
    register_api("RtlCompareString", new ApiRtlCompareString());
    register_api("RtlEqualString", new ApiRtlEqualString());
    register_api("RtlRandom", new ApiRtlRandom());
    register_api("RtlRandomEx", new ApiRtlRandomEx());
    register_api("RtlTimeToTimeFields", new ApiRtlTimeToTimeFields());
    register_api("RtlTimeFieldsToTime", new ApiRtlTimeFieldsToTime());

    // Callbacks (Anti-Cheat / EDR)
    auto* createProc = new ApiPsSetCreateProcessNotifyRoutine();
    register_api("PsSetCreateProcessNotifyRoutine", createProc);
    register_api("PsRemoveCreateProcessNotifyRoutine", createProc);
    register_api("PsSetCreateProcessNotifyRoutineEx", new ApiPsSetCreateProcessNotifyRoutineEx());
    register_api("PsSetCreateProcessNotifyRoutineEx2", new ApiPsSetCreateProcessNotifyRoutineEx2());
    register_api("PsSetCreateThreadNotifyRoutine", new ApiPsSetCreateThreadNotifyRoutine());
    register_api("PsSetCreateThreadNotifyRoutineEx", new ApiPsSetCreateThreadNotifyRoutineEx());
    register_api("PsRemoveCreateThreadNotifyRoutine", new ApiPsRemoveCreateThreadNotifyRoutine());
    register_api("PsSetLoadImageNotifyRoutine", new ApiPsSetLoadImageNotifyRoutine());
    register_api("PsRemoveLoadImageNotifyRoutine", new ApiPsRemoveLoadImageNotifyRoutine());
    register_api("ObRegisterCallbacks", new ApiObRegisterCallbacks());
    register_api("ObUnRegisterCallbacks", new ApiObUnRegisterCallbacks());
    register_api("CmRegisterCallback", new ApiCmRegisterCallback());
    register_api("CmRegisterCallbackEx", new ApiCmRegisterCallbackEx());
    register_api("CmUnRegisterCallback", new ApiCmUnRegisterCallback());

    // Process & Thread
    auto* curProc = new ApiPsGetCurrentProcess();
    register_api("PsGetCurrentProcess", curProc);
    register_api("IoGetCurrentProcess", curProc);
    register_api("PsGetCurrentProcessId", new ApiPsGetCurrentProcessId());
    register_api("PsGetCurrentThread", new ApiPsGetCurrentThread());
    register_api("PsGetCurrentThreadId", new ApiPsGetCurrentThreadId());
    register_api("PsLookupProcessByProcessId", new ApiPsLookupProcessByProcessId());
    register_api("PsLookupThreadByThreadId", new ApiPsLookupThreadByThreadId());
    register_api("PsGetProcessId", new ApiPsGetProcessId());
    register_api("PsGetThreadId", new ApiPsGetThreadId());
    register_api("PsGetProcessImageFileName", new ApiPsGetProcessImageFileName());
    register_api("PsGetProcessSectionBaseAddress", new ApiPsGetProcessSectionBaseAddress());
    register_api("PsGetProcessPeb", new ApiPsGetProcessPeb());
    register_api("PsIsProtectedProcess", new ApiPsIsProtectedProcess());
    register_api("PsIsSystemProcess", new ApiPsIsSystemProcess());
    register_api("IoThreadToProcess", new ApiIoThreadToProcess());
    register_api("PsCreateSystemThread", new ApiPsCreateSystemThread());
    register_api("PsTerminateSystemThread", new ApiPsTerminateSystemThread());

    // Device & Object Management
    register_api("IoCreateDevice", new ApiIoCreateDevice());
    register_api("IoCreateSymbolicLink", new ApiIoCreateSymbolicLink());
    register_api("IoDeleteSymbolicLink", new ApiIoDeleteSymbolicLink());
    register_api("IoDeleteDevice", new ApiIoDeleteDevice());
    auto* ioComplete = new ApiIoCompleteRequest();
    register_api("IoCompleteRequest", ioComplete);
    register_api("IofCompleteRequest", ioComplete);
    register_api("IoAttachDevice", new ApiIoAttachDevice());
    register_api("IoAttachDeviceToDeviceStack", new ApiIoAttachDeviceToDeviceStack());
    register_api("IoDetachDevice", new ApiIoDetachDevice());
    register_api("IoGetDeviceObjectPointer", new ApiIoGetDeviceObjectPointer());
    register_api("IoRegisterShutdownNotification", new ApiIoRegisterShutdownNotification());
    register_api("IoUnregisterShutdownNotification", new ApiIoUnregisterShutdownNotification());
    register_api("IoRegisterDriverReinitialization", new ApiIoRegisterDriverReinitialization());
    register_api("IoAllocateIrp", new ApiIoAllocateIrp());
    register_api("IoFreeIrp", new ApiIoFreeIrp());
    register_api("IoInitializeIrp", new ApiIoInitializeIrp());
    auto* ioCall = new ApiIoCallDriver();
    register_api("IoCallDriver", ioCall);
    register_api("IofCallDriver", ioCall);
    register_api("IoReuseIrp", new ApiIoReuseIrp());
    register_api("ObReferenceObjectByHandle", new ApiObReferenceObjectByHandle());
    register_api("ObReferenceObjectByName", new ApiObReferenceObjectByName());
    register_api("ObReferenceObject", new ApiObReferenceObject());
    auto* obDeref = new ApiObfDereferenceObject();
    register_api("ObfDereferenceObject", obDeref);
    register_api("ObDereferenceObject", obDeref);
    register_api("ObOpenObjectByPointer", new ApiObOpenObjectByPointer());
    register_api("ObGetObjectType", new ApiObGetObjectType());
    register_api("ObQueryNameString", new ApiObQueryNameString());

    // Memory & Virtual Memory
    register_api("MmIsAddressValid", new ApiMmIsAddressValid());
    register_api("MmCopyVirtualMemory", new ApiMmCopyVirtualMemory());
    register_api("MmMapIoSpace", new ApiMmMapIoSpace());
    register_api("MmMapIoSpaceEx", new ApiMmMapIoSpaceEx());
    register_api("MmUnmapIoSpace", new ApiMmUnmapIoSpace());
    register_api("IoAllocateMdl", new ApiIoAllocateMdl());
    register_api("IoFreeMdl", new ApiIoFreeMdl());
    register_api("MmBuildMdlForNonPagedPool", new ApiMmBuildMdlForNonPagedPool());
    register_api("MmProbeAndLockPages", new ApiMmProbeAndLockPages());
    register_api("MmUnlockPages", new ApiMmUnlockPages());
    register_api("MmMapLockedPagesSpecifyCache", new ApiMmMapLockedPagesSpecifyCache());
    register_api("MmMapLockedPages", new ApiMmMapLockedPages());
    register_api("MmUnmapLockedPages", new ApiMmUnmapLockedPages());
    register_api("MmAllocatePagesForMdl", new ApiMmAllocatePagesForMdl());
    register_api("MmAllocatePagesForMdlEx", new ApiMmAllocatePagesForMdlEx());
    register_api("MmFreePagesFromMdl", new ApiMmFreePagesFromMdl());
    register_api("MmAllocateContiguousMemory", new ApiMmAllocateContiguousMemory());
    register_api("MmAllocateContiguousMemorySpecifyCache", new ApiMmAllocateContiguousMemorySpecifyCache());
    register_api("MmFreeContiguousMemory", new ApiMmFreeContiguousMemory());
    register_api("MmGetPhysicalAddress", new ApiMmGetPhysicalAddress());
    register_api("MmGetSystemRoutineAddress", new ApiMmGetSystemRoutineAddress());
    register_api("MmSecureVirtualMemory", new ApiMmSecureVirtualMemory());
    register_api("MmUnsecureVirtualMemory", new ApiMmUnsecureVirtualMemory());

    // Synchronization & IRQL
    register_api("KeInitializeSpinLock", new ApiKeInitializeSpinLock());
    auto* acqSpin = new ApiKeAcquireSpinLockRaiseToDpc();
    register_api("KeAcquireSpinLockRaiseToDpc", acqSpin);
    register_api("KeAcquireSpinLock", acqSpin);
    register_api("KeReleaseSpinLock", new ApiKeReleaseSpinLock());
    register_api("KfAcquireSpinLock", new ApiKfAcquireSpinLock());
    register_api("KfReleaseSpinLock", new ApiKfReleaseSpinLock());
    register_api("KeGetCurrentIrql", new ApiKeGetCurrentIrql());
    register_api("KeRaiseIrql", new ApiKeRaiseIrql());
    register_api("KeLowerIrql", new ApiKeLowerIrql());
    register_api("KeInitializeEvent", new ApiKeInitializeEvent());
    register_api("KeSetEvent", new ApiKeSetEvent());
    register_api("KeResetEvent", new ApiKeResetEvent());
    register_api("KeClearEvent", new ApiKeClearEvent());
    register_api("KeWaitForSingleObject", new ApiKeWaitForSingleObject());
    register_api("KeWaitForMultipleObjects", new ApiKeWaitForMultipleObjects());
    register_api("KeInitializeMutex", new ApiKeInitializeMutex());
    register_api("KeReleaseMutex", new ApiKeReleaseMutex());
    register_api("KeInitializeSemaphore", new ApiKeInitializeSemaphore());
    register_api("KeReleaseSemaphore", new ApiKeReleaseSemaphore());
    register_api("ExInitializeFastMutex", new ApiExInitializeFastMutex());
    register_api("ExAcquireFastMutex", new ApiExAcquireFastMutex());
    register_api("ExReleaseFastMutex", new ApiExReleaseFastMutex());
    register_api("ExInitializePushLock", new ApiExInitializePushLock());
    register_api("ExAcquirePushLockExclusive", new ApiExAcquirePushLockExclusive());
    register_api("ExReleasePushLockExclusive", new ApiExReleasePushLockExclusive());
    register_api("ExAcquirePushLockShared", new ApiExAcquirePushLockShared());
    register_api("ExReleasePushLockShared", new ApiExReleasePushLockShared());
    register_api("KeDelayExecutionThread", new ApiKeDelayExecutionThread());
    register_api("KeQuerySystemTime", new ApiKeQuerySystemTime());
    register_api("KeQuerySystemTimePrecise", new ApiKeQuerySystemTimePrecise());
    register_api("KeQueryPerformanceCounter", new ApiKeQueryPerformanceCounter());
    register_api("KeQueryTickCount", new ApiKeQueryTickCount());
    register_api("KeQueryTimeIncrement", new ApiKeQueryTimeIncrement());
    register_api("KeInitializeTimer", new ApiKeInitializeTimer());
    register_api("KeInitializeTimerEx", new ApiKeInitializeTimerEx());
    register_api("KeSetTimer", new ApiKeSetTimer());
    register_api("KeSetTimerEx", new ApiKeSetTimerEx());
    register_api("KeCancelTimer", new ApiKeCancelTimer());
    register_api("KeInitializeDpc", new ApiKeInitializeDpc());
    register_api("KeInsertQueueDpc", new ApiKeInsertQueueDpc());
    register_api("KeRemoveQueueDpc", new ApiKeRemoveQueueDpc());
    register_api("KeQueryActiveProcessors", new ApiKeQueryActiveProcessors());
    register_api("KeQueryActiveProcessorCount", new ApiKeQueryActiveProcessorCount());
    register_api("KeQueryActiveProcessorCountEx", new ApiKeQueryActiveProcessorCountEx());

    // Zw & Nt System Calls
    auto* openKey = new ApiZwOpenKey();
    register_api("ZwOpenKey", openKey);
    register_api("NtOpenKey", openKey);

    auto* createKey = new ApiZwCreateKey();
    register_api("ZwCreateKey", createKey);
    register_api("NtCreateKey", createKey);

    auto* zwClose = new ApiZwClose();
    register_api("ZwClose", zwClose);
    register_api("NtClose", zwClose);

    auto* queryValKey = new ApiZwQueryValueKey();
    register_api("ZwQueryValueKey", queryValKey);
    register_api("NtQueryValueKey", queryValKey);

    auto* setValKey = new ApiZwSetValueKey();
    register_api("ZwSetValueKey", setValKey);
    register_api("NtSetValueKey", setValKey);

    auto* delKey = new ApiZwDeleteKey();
    register_api("ZwDeleteKey", delKey);
    register_api("NtDeleteKey", delKey);

    auto* delValKey = new ApiZwDeleteValueKey();
    register_api("ZwDeleteValueKey", delValKey);
    register_api("NtDeleteValueKey", delValKey);

    auto* enumKey = new ApiZwEnumerateKey();
    register_api("ZwEnumerateKey", enumKey);
    register_api("NtEnumerateKey", enumKey);

    auto* enumValKey = new ApiZwEnumerateValueKey();
    register_api("ZwEnumerateValueKey", enumValKey);
    register_api("NtEnumerateValueKey", enumValKey);

    register_api("RtlQueryRegistryValues", new ApiRtlQueryRegistryValues());

    auto* openFile = new ApiZwOpenFile();
    register_api("ZwOpenFile", openFile);
    register_api("NtOpenFile", openFile);

    auto* createFile = new ApiZwCreateFile();
    register_api("ZwCreateFile", createFile);
    register_api("NtCreateFile", createFile);

    auto* readFile = new ApiZwReadFile();
    register_api("ZwReadFile", readFile);
    register_api("NtReadFile", readFile);

    auto* writeFile = new ApiZwWriteFile();
    register_api("ZwWriteFile", writeFile);
    register_api("NtWriteFile", writeFile);

    auto* queryInfoFile = new ApiZwQueryInformationFile();
    register_api("ZwQueryInformationFile", queryInfoFile);
    register_api("NtQueryInformationFile", queryInfoFile);

    auto* setInfoFile = new ApiZwSetInformationFile();
    register_api("ZwSetInformationFile", setInfoFile);
    register_api("NtSetInformationFile", setInfoFile);

    auto* querySysInfo = new ApiZwQuerySystemInformation();
    register_api("ZwQuerySystemInformation", querySysInfo);
    register_api("NtQuerySystemInformation", querySysInfo);

    auto* queryInfoProc = new ApiZwQueryInformationProcess();
    register_api("ZwQueryInformationProcess", queryInfoProc);
    register_api("NtQueryInformationProcess", queryInfoProc);

    auto* setInfoProc = new ApiZwSetInformationProcess();
    register_api("ZwSetInformationProcess", setInfoProc);
    register_api("NtSetInformationProcess", setInfoProc);

    auto* queryInfoThread = new ApiZwQueryInformationThread();
    register_api("ZwQueryInformationThread", queryInfoThread);
    register_api("NtQueryInformationThread", queryInfoThread);

    auto* setInfoThread = new ApiZwSetInformationThread();
    register_api("ZwSetInformationThread", setInfoThread);
    register_api("NtSetInformationThread", setInfoThread);

    auto* openProc = new ApiZwOpenProcess();
    register_api("ZwOpenProcess", openProc);
    register_api("NtOpenProcess", openProc);

    auto* openThread = new ApiZwOpenThread();
    register_api("ZwOpenThread", openThread);
    register_api("NtOpenThread", openThread);

    auto* termProc = new ApiZwTerminateProcess();
    register_api("ZwTerminateProcess", termProc);
    register_api("NtTerminateProcess", termProc);

    auto* allocVM = new ApiZwAllocateVirtualMemory();
    register_api("ZwAllocateVirtualMemory", allocVM);
    register_api("NtAllocateVirtualMemory", allocVM);

    auto* freeVM = new ApiZwFreeVirtualMemory();
    register_api("ZwFreeVirtualMemory", freeVM);
    register_api("NtFreeVirtualMemory", freeVM);

    auto* protVM = new ApiZwProtectVirtualMemory();
    register_api("ZwProtectVirtualMemory", protVM);
    register_api("NtProtectVirtualMemory", protVM);

    auto* readVM = new ApiZwReadVirtualMemory();
    register_api("ZwReadVirtualMemory", readVM);
    register_api("NtReadVirtualMemory", readVM);

    auto* writeVM = new ApiZwWriteVirtualMemory();
    register_api("ZwWriteVirtualMemory", writeVM);
    register_api("NtWriteVirtualMemory", writeVM);

    // Minifilter
    register_api("FltRegisterFilter", new ApiFltRegisterFilter());
    register_api("FltUnregisterFilter", new ApiFltUnregisterFilter());
    register_api("FltStartFiltering", new ApiFltStartFiltering());
    register_api("FltGetFileNameInformation", new ApiFltGetFileNameInformation());
    register_api("FltReleaseFileNameInformation", new ApiFltReleaseFileNameInformation());
    register_api("FltParseFileNameInformation", new ApiFltParseFileNameInformation());

    // ETW & HAL
    register_api("EtwRegister", new ApiEtwRegister());
    register_api("EtwUnregister", new ApiEtwUnregister());
    register_api("EtwWrite", new ApiEtwWrite());
    register_api("HalGetBusData", new ApiHalGetBusData());
    register_api("HalSetBusData", new ApiHalSetBusData());

    // CNG / BCrypt
    register_api("BCryptOpenAlgorithmProvider", new ApiBCryptOpenAlgorithmProvider());
    register_api("BCryptCreateHash",            new ApiBCryptCreateHash());
    register_api("BCryptHashData",              new ApiBCryptHashData());
    register_api("BCryptFinishHash",            new ApiBCryptFinishHash());
    register_api("BCryptDestroyHash",           new ApiBCryptDestroyHash());
    register_api("BCryptCloseAlgorithmProvider",new ApiBCryptCloseAlgorithmProvider());
}

void APIDispatcher::register_api(const std::string &name, Api *api){
    apis[name] = api;
}

void emulate_ret(CPU* cpu, uint64_t val){
    // simulate ret (return), which is stored in rax
    cpu->set_register(REG_RAX, val);

    // pop return addres from stack and jump
    uint64_t rsp = cpu->get_register(REG_RSP);
    uint64_t ret_addr = 0;
    cpu->mem_read(rsp, &ret_addr, sizeof(uint64_t));
    cpu->set_register(REG_RSP, rsp + 8);
    cpu->set_register(REG_RIP, ret_addr);
}

void APIDispatcher::invoke(const std::string& name, CPU* cpu) {
    auto it = apis.find(name);
    uint64_t ret_val = 0;
    if(it != apis.end()) {
        ret_val = it->second->call(cpu);
    }

    else{
        Debug::debug_msg("Stub " + name + " not implemented, returning 0", LOG_WARN);
    }

    emulate_ret(cpu, ret_val);
}

bool APIDispatcher::resolve(CPU *cpu){
    uint64_t index = (cpu->get_register(REG_RIP) - HOOK_TRAP_BASE) / 0x10; // hooks are mapped with (hook base + (0x10 * i))
    
    TrapEntry call = cpu->trap_table.at(index);
    
    Debug::debug_msg("API_CALL[fn= " + call.function_name + ", module= " + call.module_name + "]", LOG_WARN);

    invoke(call.function_name, cpu);
    return true;
}