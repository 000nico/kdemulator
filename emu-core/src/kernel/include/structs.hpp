#pragma once
#include <cstdint>


#ifdef DWORD
#undef DWORD
#endif
#ifdef WORD
#undef WORD
#endif
#ifdef BYTE
#undef BYTE
#endif

#ifndef _WINDOWS_TYPES_DEFINED
#define _WINDOWS_TYPES_DEFINED

using UCHAR     = uint8_t;
using CCHAR     = char;
using CSHORT    = int16_t;
using USHORT    = uint16_t;
using SHORT     = int16_t;
using ULONG     = uint32_t;
using LONG      = int32_t;
using ULONG64   = uint64_t;
using LONG64    = int64_t;
using ULONGLONG = uint64_t;
using LONGLONG  = int64_t;
using ULONG_PTR = uint64_t;
using LONG_PTR  = int64_t;
using PVOID     = void*;
using BOOLEAN   = uint8_t;
using WCHAR     = uint16_t;
using KAFFINITY = uint64_t;

using PWSTR  = WCHAR*;
using PCWSTR = const WCHAR*;
using PSTR   = char*;
using PCSTR  = const char*;

using KSPIN_LOCK         = ULONG_PTR;
using PSECURITY_DESCRIPTOR = PVOID;
using DEVICE_TYPE        = ULONG;

struct LIST_ENTRY {
    LIST_ENTRY* Flink;
    LIST_ENTRY* Blink;
};

struct UNICODE_STRING {
    USHORT   Length;
    USHORT   MaximumLength;
    WCHAR*   Buffer;
};

union LARGE_INTEGER {
    struct { ULONG LowPart; LONG HighPart; } u;
    LONGLONG QuadPart;
};

union ULARGE_INTEGER {
    struct { ULONG LowPart; ULONG HighPart; } u;
    ULONGLONG QuadPart;
};

struct KEVENT        { UCHAR _opaque[24]; };
struct KDPC          { UCHAR _opaque[64]; };
struct KDEVICE_QUEUE { UCHAR _opaque[32]; };
struct WAIT_CONTEXT_BLOCK { UCHAR _opaque[48]; };

struct _DRIVER_OBJECT;
struct _DEVICE_OBJECT;
struct _IRP;
struct _VPB;
struct _IO_TIMER;
struct _DRIVER_EXTENSION;
struct _FAST_IO_DISPATCH;

using PDRIVER_OBJECT = _DRIVER_OBJECT*;
using PDEVICE_OBJECT = _DEVICE_OBJECT*;
using PIRP           = _IRP*;
using PVPB           = _VPB*;

using PDRIVER_INITIALIZE = LONG (*)(PDRIVER_OBJECT, UNICODE_STRING*);
using PDRIVER_STARTIO    = void (*)(PDEVICE_OBJECT, PIRP);
using PDRIVER_UNLOAD     = void (*)(PDRIVER_OBJECT);
using PDRIVER_DISPATCH   = LONG (*)(PDEVICE_OBJECT, PIRP);

#define IRP_MJ_MAXIMUM_FUNCTION     0x1b
#define FILE_BYTE_ALIGNMENT         0x00000000
#define DO_BUFFERED_IO              0x00000004
#define DO_EXCLUSIVE                0x00000008
#define DO_DIRECT_IO                0x00000010
#define FILE_DEVICE_UNKNOWN         0x00000022
#define DRVO_UNLOAD_INVOKED         0x00000001
#define DRVO_LEGACY_DRIVER          0x00000002
#define DRVO_BUILTIN_DRIVER         0x00000004
#define DRVO_REINIT_REGISTERED      0x00000008
#define DRVO_INITIALIZED            0x00000010
#define DRVO_BOOTREINIT_REGISTERED  0x00000020
#define DRVO_DPAGEABLE              0x00000040

struct _DRIVER_OBJECT {
    CSHORT             Type;
    CSHORT             Size;
    PDEVICE_OBJECT     DeviceObject;
    ULONG              Flags;
    PVOID              DriverStart;
    ULONG              DriverSize;
    PVOID              DriverSection;
    _DRIVER_EXTENSION* DriverExtension;
    UNICODE_STRING     DriverName;
    UNICODE_STRING*    HardwareDatabase;
    _FAST_IO_DISPATCH* FastIoDispatch;
    PDRIVER_INITIALIZE DriverInit;
    PDRIVER_STARTIO    DriverStartIo;
    PDRIVER_UNLOAD     DriverUnload;
    PDRIVER_DISPATCH   MajorFunction[IRP_MJ_MAXIMUM_FUNCTION + 1];
};
using DRIVER_OBJECT = _DRIVER_OBJECT;

struct _DRIVER_EXTENSION {
    PDRIVER_OBJECT DriverObject;
    PVOID          AddDevice;
    ULONG          Count;
    UNICODE_STRING ServiceKeyName;
};

struct _FAST_IO_DISPATCH { UCHAR _opaque[0]; };

struct _DEVICE_OBJECT {
    CSHORT               Type;
    USHORT               Size;
    LONG                 ReferenceCount;
    PDRIVER_OBJECT       DriverObject;
    PDEVICE_OBJECT       NextDevice;
    PDEVICE_OBJECT       AttachedDevice;
    PIRP                 CurrentIrp;
    _IO_TIMER*           Timer;
    ULONG                Flags;
    ULONG                Characteristics;
    PVPB                 Vpb;
    PVOID                DeviceExtension;
    DEVICE_TYPE          DeviceType;
    CCHAR                StackSize;
    union {
        LIST_ENTRY         ListEntry;
        WAIT_CONTEXT_BLOCK Wcb;
    } Queue;
    ULONG                AlignmentRequirement;
    KDEVICE_QUEUE        DeviceQueue;
    KDPC                 Dpc;
    ULONG                ActiveThreadCount;
    PSECURITY_DESCRIPTOR SecurityDescriptor;
    KEVENT               DeviceLock;
    USHORT               SectorSize;
    USHORT               Spare1;
    PVOID                DeviceObjectExtension;
    PVOID                Reserved;
};
using DEVICE_OBJECT = _DEVICE_OBJECT;

struct _IO_TIMER { UCHAR _opaque[0]; };
struct _VPB      { UCHAR _opaque[0]; };

#pragma pack(push, 1)

struct KSYSTEM_TIME {
    ULONG LowPart;
    LONG  High1Time;
    LONG  High2Time;
};

enum NT_PRODUCT_TYPE : ULONG {
    NtProductWinNt    = 1,
    NtProductLanManNt = 2,
    NtProductServer   = 3,
};

enum ALTERNATIVE_ARCHITECTURE_TYPE : ULONG {
    StandardDesign = 0,
    NEC98x86,
    EndAlternatives,
};

struct KUSER_SHARED_DATA {
    ULONG          TickCountLowDeprecated;
    ULONG          TickCountMultiplier;
    KSYSTEM_TIME   InterruptTime;
    KSYSTEM_TIME   SystemTime;
    KSYSTEM_TIME   TimeZoneBias;
    USHORT         ImageNumberLow;
    USHORT         ImageNumberHigh;
    WCHAR          NtSystemRoot[260];
    ULONG          MaxStackTraceDepth;
    ULONG          CryptoExponent;
    ULONG          TimeZoneId;
    ULONG          LargePageMinimum;
    ULONG          AitSamplingValue;
    ULONG          AppCompatFlag;
    ULONGLONG      RNGSeedVersion;
    ULONG          GlobalValidationRunlevel;
    LONG           TimeZoneBiasStamp;
    ULONG          NtBuildNumber;
    NT_PRODUCT_TYPE NtProductType;
    BOOLEAN        ProductTypeIsValid;
    BOOLEAN        Reserved0[1];
    USHORT         NativeProcessorArchitecture;
    ULONG          NtMajorVersion;
    ULONG          NtMinorVersion;
    BOOLEAN        ProcessorFeatures[64];
    ULONG          Reserved1;
    ULONG          Reserved3;
    ULONG          TimeSlip;
    ALTERNATIVE_ARCHITECTURE_TYPE AlternativeArchitecture;
    ULONG          AltArchitecturePad[1];
    LARGE_INTEGER  SystemExpirationDate;
    ULONG          SuiteMask;
    BOOLEAN        KdDebuggerEnabled;
    UCHAR          MitigationPolicies;
    USHORT         CyclesPerYield;
    ULONG          ActiveConsoleId;
    ULONG          DismountCount;
    ULONG          ComPlusPackage;
    ULONG          LastSystemRITEventTickCount;
    ULONG          NumberOfPhysicalPages;
    BOOLEAN        SafeBootMode;
    UCHAR          VirtualizationFlags;
    UCHAR          Reserved12[2];
    ULONG          SharedDataFlags;
    ULONG          DataFlagsPad[1];
    ULONGLONG      TestRetInstruction;
    LONGLONG       QpcFrequency;
    ULONG          SystemCall;
    ULONG          Reserved2;
    ULONGLONG      FullNumberOfPhysicalPages;
    ULONGLONG      SystemCallPad[1];
    union {
        KSYSTEM_TIME TickCount;
        ULONG64      TickCountQuad;
        struct { ULONG ReservedTickCountOverlay[3]; ULONG TickCountPad[1]; };
    };
    ULONG          Cookie;
    ULONG          CookiePad[1];
    LONGLONG       ConsoleSessionForegroundProcessId;
    ULONGLONG      TimeUpdateLock;
    ULONGLONG      BaselineSystemTimeQpc;
    ULONGLONG      BaselineInterruptTimeQpc;
    ULONGLONG      QpcSystemTimeIncrement;
    ULONGLONG      QpcInterruptTimeIncrement;
    UCHAR          QpcSystemTimeIncrementShift;
    UCHAR          QpcInterruptTimeIncrementShift;
    USHORT         UnparkedProcessorCount;
    ULONG          EnclaveFeatureMask[4];
    ULONG          TelemetryCoverageRound;
    USHORT         UserModeGlobalLogger[16];
    ULONG          ImageFileExecutionOptions;
    ULONG          LangGenerationCount;
    ULONGLONG      Reserved4;
    ULONGLONG      InterruptTimeBias;
    ULONGLONG      QpcBias;
    ULONG          ActiveProcessorCount;
    UCHAR          ActiveGroupCount;
    UCHAR          Reserved9;
    union { USHORT QpcData; struct { UCHAR QpcBypassEnabled; UCHAR QpcShift; }; };
    LARGE_INTEGER  TimeZoneBiasEffectiveStart;
    LARGE_INTEGER  TimeZoneBiasEffectiveEnd;
    UCHAR          _XStatePlaceholder[384];
    KSYSTEM_TIME   FeatureConfigurationChangeStamp;
    ULONG          Spare;
    ULONG64        UserPointerAuthMask;
    UCHAR          Reserved5[0x1000 - 0x570];
};

#pragma pack(pop)

#pragma pack(push, 1) 

struct IO_STACK_LOCATION {
    uint8_t  MajorFunction;        // +0x00  -- IRP_MJ_DEVICE_CONTROL = 0x0E, etc.
    uint8_t  MinorFunction;        // +0x01
    uint8_t  Flags;                // +0x02
    uint8_t  Control;              // +0x03
    uint32_t _pad0;                // +0x04  

    // === union Parameters (offset +0x08) ===
    uint32_t OutputBufferLength;   // +0x08  Parameters.DeviceIoControl.OutputBufferLength
    uint32_t POINTER_ALIGNMENT;    // +0x0C  (padding real que usa el compilador acá)
    uint32_t InputBufferLength;    // +0x10  Parameters.DeviceIoControl.InputBufferLength
    uint32_t _pad1;                // +0x14
    uint32_t IoControlCode;        // +0x18  Parameters.DeviceIoControl.IoControlCode
    uint32_t _pad2;                // +0x1C
    uint64_t Type3InputBuffer;     // +0x20  Parameters.DeviceIoControl.Type3InputBuffer (METHOD_NEITHER)
    // === fin union ===

    uint64_t DeviceObject;         // +0x28  PDEVICE_OBJECT
    uint64_t FileObject;           // +0x30  PFILE_OBJECT
    uint64_t CompletionRoutine;    // +0x38  PIO_COMPLETION_ROUTINE
    uint64_t Context;              // +0x40
};
// sizeof(IO_STACK_LOCATION) == 0x48 

#pragma pack(pop)

#pragma pack(push, 1)

struct IRP {
    uint16_t Type;                     // +0x00  = 6 (IO_TYPE_IRP)
    uint16_t Size;                     // +0x02  = sizeof(IRP) + N * sizeof(IO_STACK_LOCATION)
    uint32_t _pad0;                    // +0x04

    uint64_t MdlAddress;               // +0x08  PMDL, usado en METHOD_IN/OUT_DIRECT
    uint32_t Flags;                    // +0x10  IRP_BUFFERED_IO, IRP_INPUT_OPERATION, etc.
    uint32_t _pad1;                    // +0x14

    uint64_t AssociatedIrp_SystemBuffer; // +0x18  AssociatedIrp.SystemBuffer -- METHOD_BUFFERED

    // ThreadListEntry (LIST_ENTRY, 16 bytes) -- no la necesitás simular
    uint64_t ThreadListEntry_Flink;    // +0x20
    uint64_t ThreadListEntry_Blink;    // +0x28

    // IoStatus (IO_STATUS_BLOCK, 16 bytes) -- ESTO SÍ LO NECESITÁS
    int64_t  IoStatus_Status;          // +0x30  NTSTATUS 
    uint64_t IoStatus_Information;     // +0x38 

    uint8_t  RequestorMode;            // +0x40  KernelMode = 0, UserMode = 1
    uint8_t  PendingReturned;          // +0x41
    uint8_t  StackCount;               // +0x42
    uint8_t  CurrentLocation;          // +0x43
    uint8_t  Cancel;                   // +0x44
    uint8_t  CancelIrql;               // +0x45
    uint8_t  ApcEnvironment;           // +0x46
    uint8_t  AllocationFlags;          // +0x47

    uint64_t UserIosb;                 // +0x48  PIO_STATUS_BLOCK 
    uint64_t UserEvent;                // +0x50  PKEVENT
    uint64_t Overlay_AsynchronousParameters_UserApcRoutine; // +0x58 
    uint64_t Overlay_AllocationSize_or_ApcContext;          // +0x60

    uint64_t CancelRoutine;            // +0x68  PDRIVER_CANCEL
    uint64_t UserBuffer;               // +0x70  PVOID -- METHOD_NEITHER

    // Tail.Overlay -- ponmting to active IO_STACK_LOCATION 
    uint64_t Tail_Overlay_DeviceQueueEntry_or_DriverContext[4]; // +0x78 (unión, 32 bytes)
    uint64_t Tail_Overlay_Thread;                                // +0x98
    uint64_t Tail_Overlay_AuxiliaryBuffer;                       // +0xA0
    uint64_t Tail_Overlay_ListEntry_Flink;                       // +0xA8
    uint64_t Tail_Overlay_ListEntry_Blink;                       // +0xB0
    uint64_t Tail_Overlay_CurrentStackLocation;                  // +0xB8  <-- PIO_STACK_LOCATION,
    uint64_t Tail_Overlay_OriginalFileObject;                    // +0xC0
};
// sizeof(IRP) real == 0xD0 but we could use a bit of margin

#pragma pack(pop)

inline KUSER_SHARED_DATA* const UserSharedData =
    reinterpret_cast<KUSER_SHARED_DATA*>(0x7FFE0000);

#endif // _WINDOWS_TYPES_DEFINED