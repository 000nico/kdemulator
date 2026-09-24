#pragma once
#include <cstdint>

using UCHAR    = uint8_t;
using CCHAR    = char;
using CSHORT   = int16_t;
using USHORT   = uint16_t;
using SHORT    = int16_t;
using ULONG    = uint32_t;
using LONG     = int32_t;
using ULONG64  = uint64_t;
using LONG64   = int64_t;
using ULONGLONG = uint64_t;
using LONGLONG  = int64_t;
using ULONG_PTR = uint64_t;   // x64
using LONG_PTR  = int64_t;
using PVOID     = void*;
using BOOLEAN   = UCHAR;

using WCHAR     = uint16_t;
using KAFFINITY = ULONG_PTR;

struct LIST_ENTRY {
    LIST_ENTRY* Flink;
    LIST_ENTRY* Blink;
};

struct UNICODE_STRING {
    USHORT  Length;
    USHORT  MaximumLength;
    WCHAR*  Buffer;
};


union LARGE_INTEGER {
    struct { ULONG LowPart; LONG HighPart; } u;
    LONGLONG QuadPart;
};

union ULARGE_INTEGER {
    struct { ULONG LowPart; ULONG HighPart; } u;
    ULONGLONG QuadPart;
};

// KSPIN_LOCK real es solo un ULONG_PTR
using KSPIN_LOCK = ULONG_PTR;

struct KEVENT       { UCHAR _opaque[24]; };   // DISPATCHER_HEADER + list
struct KDPC         { UCHAR _opaque[64]; };
struct KDEVICE_QUEUE{ UCHAR _opaque[32]; };
struct DEVOBJ_EXTENSION_OPAQUE { UCHAR _opaque[0]; }; 
struct WAIT_CONTEXT_BLOCK { UCHAR _opaque[48]; };

using PSECURITY_DESCRIPTOR = PVOID;
using DEVICE_TYPE = ULONG;

// ---------------------------------------------------------------------
// Forward decls
// ---------------------------------------------------------------------
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
using PVPB            = _VPB*;


using PDRIVER_INITIALIZE = LONG (*)(PDRIVER_OBJECT, UNICODE_STRING*);
using PDRIVER_STARTIO    = void (*)(PDEVICE_OBJECT, PIRP);
using PDRIVER_UNLOAD     = void (*)(PDRIVER_OBJECT);
using PDRIVER_DISPATCH   = LONG (*)(PDEVICE_OBJECT, PIRP);

#define IRP_MJ_MAXIMUM_FUNCTION 0x1b   // igual que en wdm.h


struct _DRIVER_OBJECT {
    CSHORT              Type;                 // siempre 4 (IO_TYPE_DRIVER)
    CSHORT              Size;
    PDEVICE_OBJECT      DeviceObject;          // head de la lista enlazada de DEVICE_OBJECT (via NextDevice)
    ULONG               Flags;
    PVOID               DriverStart;           // base del .sys en memoria
    ULONG               DriverSize;            // tamaño de la imagen
    PVOID               DriverSection;         // en realidad un PLDR_DATA_TABLE_ENTRY oculto
    _DRIVER_EXTENSION*  DriverExtension;
    UNICODE_STRING      DriverName;            // ej: \Driver\MiDriver
    UNICODE_STRING*     HardwareDatabase;       // -> \Registry\Machine\Hardware\Description\System
    _FAST_IO_DISPATCH*  FastIoDispatch;
    PDRIVER_INITIALIZE  DriverInit;             // puntero a DriverEntry
    PDRIVER_STARTIO     DriverStartIo;
    PDRIVER_UNLOAD      DriverUnload;
    PDRIVER_DISPATCH    MajorFunction[IRP_MJ_MAXIMUM_FUNCTION + 1]; // IRP_MJ_CREATE, IRP_MJ_DEVICE_CONTROL, etc.
};
using DRIVER_OBJECT = _DRIVER_OBJECT;

struct _DRIVER_EXTENSION {
    PDRIVER_OBJECT  DriverObject;
    PVOID           AddDevice;                 // PDRIVER_ADD_DEVICE
    ULONG           Count;
    UNICODE_STRING  ServiceKeyName;
    // ... IoClientExtension / IoDeviceInterfaceKeyName / DriverPoolTag
    // se omiten por brevedad, no forman parte de la pregunta.
};

struct _FAST_IO_DISPATCH { UCHAR _opaque[0]; }; // se referencia por puntero, no hace falta cuerpo

struct _DEVICE_OBJECT {
    CSHORT              Type;                  // siempre 3 (IO_TYPE_DEVICE)
    USHORT              Size;
    LONG                ReferenceCount;
    PDRIVER_OBJECT      DriverObject;           // dueño de este device
    PDEVICE_OBJECT      NextDevice;             // siguiente en la lista del driver
    PDEVICE_OBJECT      AttachedDevice;         // top del stack si alguien hizo IoAttachDevice
    PIRP                CurrentIrp;             // IRP activa si el device es de tipo "serialized"
    _IO_TIMER*          Timer;
    ULONG               Flags;                  // DO_BUFFERED_IO, DO_DIRECT_IO, DO_EXCLUSIVE...
    ULONG               Characteristics;        // FILE_DEVICE_SECURE_OPEN, etc.
    PVPB                Vpb;                    // solo dispositivos de FS/volumen
    PVOID               DeviceExtension;        // memoria privada del driver (tu struct custom)
    DEVICE_TYPE         DeviceType;             // FILE_DEVICE_UNKNOWN, FILE_DEVICE_DISK, etc.
    CCHAR               StackSize;              // alto del IRP stack para este device
    union {
        LIST_ENTRY           ListEntry;
        WAIT_CONTEXT_BLOCK    Wcb;
    } Queue;
    ULONG               AlignmentRequirement;
    KDEVICE_QUEUE       DeviceQueue;
    KDPC                Dpc;
    ULONG               ActiveThreadCount;
    PSECURITY_DESCRIPTOR SecurityDescriptor;
    KEVENT              DeviceLock;
    USHORT              SectorSize;
    USHORT              Spare1;
    PVOID               DeviceObjectExtension;  // -> DEVOBJ_EXTENSION real (opaco)
    PVOID               Reserved;
};
using DEVICE_OBJECT = _DEVICE_OBJECT;

struct _IO_TIMER { UCHAR _opaque[0]; };
struct _VPB      { UCHAR _opaque[0]; };

// =====================================================================
// KUSER_SHARED_DATA — la MISMA página física que el kernel mapea en
// 0xFFFFF78000000000 (kernel-mode) y que usermode ve, read-only, en
// la dirección FIJA 0x7FFE0000 en TODO proceso x64 (KADDRESS constante,
// no ASLR). Un driver la lee directo desde el puntero de kernel; en
// usermode podés castear directamente (PVOID)0x7FFE0000 a esta struct
// sin ningún driver, porque ya está mapeada en tu propio proceso.
//
// Offsets verificados contra el layout público de 10.0.22621 (phnt /
// Geoff Chappell). Se listan los campos "estables" documentados desde
// hace años; los reservados intermedios se dejan como padding.
// =====================================================================
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
    ULONG          TickCountLowDeprecated;             // 0x000
    ULONG          TickCountMultiplier;                // 0x004
    KSYSTEM_TIME   InterruptTime;                       // 0x008
    KSYSTEM_TIME   SystemTime;                          // 0x014
    KSYSTEM_TIME   TimeZoneBias;                         // 0x020
    USHORT         ImageNumberLow;                       // 0x02C
    USHORT         ImageNumberHigh;                      // 0x02E
    WCHAR          NtSystemRoot[260];                    // 0x030  -> "C:\Windows"
    ULONG          MaxStackTraceDepth;                   // 0x238
    ULONG          CryptoExponent;                       // 0x23C
    ULONG          TimeZoneId;                            // 0x240
    ULONG          LargePageMinimum;                     // 0x244
    ULONG          AitSamplingValue;                     // 0x248
    ULONG          AppCompatFlag;                         // 0x24C
    ULONGLONG      RNGSeedVersion;                        // 0x250
    ULONG          GlobalValidationRunlevel;              // 0x258
    LONG           TimeZoneBiasStamp;                     // 0x25C
    ULONG          NtBuildNumber;                          // 0x260 -> ej. 22621
    NT_PRODUCT_TYPE NtProductType;                         // 0x264
    BOOLEAN        ProductTypeIsValid;                     // 0x268
    BOOLEAN        Reserved0[1];                            // 0x269
    USHORT         NativeProcessorArchitecture;             // 0x26A
    ULONG          NtMajorVersion;                          // 0x26C -> 10
    ULONG          NtMinorVersion;                          // 0x270 -> 0
    BOOLEAN        ProcessorFeatures[64];                   // 0x274 -> PF_XMMI_INSTRUCTIONS_AVAILABLE, etc.
    ULONG          Reserved1;                                // 0x2B4
    ULONG          Reserved3;                                // 0x2B8
    ULONG          TimeSlip;                                 // 0x2BC
    ALTERNATIVE_ARCHITECTURE_TYPE AlternativeArchitecture;    // 0x2C0
    ULONG          AltArchitecturePad[1];                     // 0x2C4
    LARGE_INTEGER  SystemExpirationDate;                       // 0x2C8
    ULONG          SuiteMask;                                  // 0x2D0
    BOOLEAN        KdDebuggerEnabled;                          // 0x2D4
    UCHAR          MitigationPolicies;                          // 0x2D5
    USHORT         CyclesPerYield;                               // 0x2D6
    ULONG          ActiveConsoleId;                              // 0x2D8
    ULONG          DismountCount;                                 // 0x2DC
    ULONG          ComPlusPackage;                                 // 0x2E0
    ULONG          LastSystemRITEventTickCount;                    // 0x2E4
    ULONG          NumberOfPhysicalPages;                          // 0x2E8
    BOOLEAN        SafeBootMode;                                    // 0x2EC
    UCHAR          VirtualizationFlags;                              // 0x2ED
    UCHAR          Reserved12[2];                                    // 0x2EE
    ULONG          SharedDataFlags;                                   // 0x2F0  (DbgErrorPortPresent, LKG, etc.)
    ULONG          DataFlagsPad[1];                                    // 0x2F4
    ULONGLONG      TestRetInstruction;                                 // 0x2F8
    LONGLONG       QpcFrequency;                                        // 0x300
    ULONG          SystemCall;                                          // 0x308 (legacy, no se usa en x64)
    ULONG          Reserved2;                                            // 0x30C
    ULONGLONG      FullNumberOfPhysicalPages;                             // 0x310
    ULONGLONG      SystemCallPad[1];                                       // 0x318
    union {
        KSYSTEM_TIME TickCount;                                              // 0x320
        ULONG64      TickCountQuad;
        struct { ULONG ReservedTickCountOverlay[3]; ULONG TickCountPad[1]; };
    };
    ULONG          Cookie;                                                    // 0x330
    ULONG          CookiePad[1];                                              // 0x334
    LONGLONG       ConsoleSessionForegroundProcessId;                          // 0x338
    ULONGLONG      TimeUpdateLock;                                              // 0x340
    ULONGLONG      BaselineSystemTimeQpc;                                        // 0x348
    ULONGLONG      BaselineInterruptTimeQpc;                                      // 0x350
    ULONGLONG      QpcSystemTimeIncrement;                                         // 0x358
    ULONGLONG      QpcInterruptTimeIncrement;                                       // 0x360
    UCHAR          QpcSystemTimeIncrementShift;                                      // 0x368
    UCHAR          QpcInterruptTimeIncrementShift;                                    // 0x369
    USHORT         UnparkedProcessorCount;                                             // 0x36A
    ULONG          EnclaveFeatureMask[4];                                               // 0x36C
    ULONG          TelemetryCoverageRound;                                               // 0x37C
    USHORT         UserModeGlobalLogger[16];                                              // 0x380
    ULONG          ImageFileExecutionOptions;                                              // 0x3A0
    ULONG          LangGenerationCount;                                                    // 0x3A4
    ULONGLONG      Reserved4;                                                               // 0x3A8
    ULONGLONG      InterruptTimeBias;                                                        // 0x3B0
    ULONGLONG      QpcBias;                                                                    // 0x3B8
    ULONG          ActiveProcessorCount;                                                        // 0x3C0
    UCHAR          ActiveGroupCount;                                                             // 0x3C4
    UCHAR          Reserved9;                                                                     // 0x3C5
    union { USHORT QpcData; struct { UCHAR QpcBypassEnabled; UCHAR QpcShift; }; };                 // 0x3C6
    LARGE_INTEGER  TimeZoneBiasEffectiveStart;                                                       // 0x3C8
    LARGE_INTEGER  TimeZoneBiasEffectiveEnd;                                                          // 0x3D0
    // XSTATE_CONFIGURATION XState;   // 0x3D8 — se omite (struct grande aparte, AVX/AVX512 etc.)
    UCHAR          _XStatePlaceholder[384];                                                            // 0x3D8
    KSYSTEM_TIME   FeatureConfigurationChangeStamp;                                                     // 0x558
    ULONG          Spare;                                                                                // 0x564
    ULONG64        UserPointerAuthMask;                                                                   // 0x568
    // resto reservado hasta completar la página de 4KB (0x1000)
    UCHAR          Reserved5[0x1000 - 0x570];
};
#pragma pack(pop)

inline KUSER_SHARED_DATA* const UserSharedData =
    reinterpret_cast<KUSER_SHARED_DATA*>(0x7FFE0000);

#define DRVO_UNLOAD_INVOKED         0x00000001  
#define DRVO_LEGACY_DRIVER          0x00000002  
#define DRVO_BUILTIN_DRIVER         0x00000004  
#define DRVO_REINIT_REGISTERED      0x00000008  
#define DRVO_INITIALIZED            0x00000010 
#define DRVO_BOOTREINIT_REGISTERED  0x00000020  
#define DRVO_DPAGEABLE              0x00000040  