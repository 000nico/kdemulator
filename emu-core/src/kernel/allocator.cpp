#include "../cpu/CPU.hpp"
#include "../cpu/perms.hpp"
#include "include/structs.hpp"
#include "../memory/layout.hpp"
#include "../pe/format.hpp"
#include "../debug/Debug.hpp"
#include <string>
#include <vector>

#define DO_BUFFERED_IO      0x00000004
#define DO_DIRECT_IO        0x00000010
#define FILE_DEVICE_UNKNOWN 0x00000022
#define FILE_BYTE_ALIGNMENT 0x00000000

constexpr uint64_t OFF_DRVOBJ  = 0x000;
constexpr uint64_t OFF_DRVEXT  = 0x150;
constexpr uint64_t OFF_DRVNAME = 0x178;
constexpr uint64_t OFF_HWDB    = 0x1F8;
constexpr uint64_t OFF_DEFDISP = 0x2F8;

static inline size_t align_up_4k(size_t size) {
    return (size + 0xFFF) & ~static_cast<size_t>(0xFFF);
}

static inline void mw(CPU* cpu, uint64_t addr, const void* data, size_t size) {
    cpu->mem_write(addr, const_cast<void*>(data), size);
}

static std::vector<uint16_t> to_utf16(const std::string& s) {
    std::vector<uint16_t> out;
    for (char c : s) out.push_back(static_cast<uint16_t>(c));
    out.push_back(0);
    return out;
}

static std::string pe_module_name(PE* pe) {
    uint32_t va = pe->image_optional_header.ExportDirectoryVA;
    if (!va) return "\\Driver\\Unknown";
    for (auto& sec : pe->sections) {
        if (va < sec.VirtualAddress || va >= sec.VirtualAddress + sec.PhysicalAddress_VirtualSize)
            continue;
        uint32_t off = sec.PointerToRawData + (va - sec.VirtualAddress);
        auto* exp = reinterpret_cast<IMAGE_EXPORT_DIRECTORY*>(pe->raw_data.data() + off);
        uint32_t name_rva = exp->Name;
        for (auto& s2 : pe->sections) {
            if (name_rva < s2.VirtualAddress || name_rva >= s2.VirtualAddress + s2.PhysicalAddress_VirtualSize)
                continue;
            uint32_t noff = s2.PointerToRawData + (name_rva - s2.VirtualAddress);
            std::string name = reinterpret_cast<char*>(pe->raw_data.data() + noff);
            auto dot = name.rfind('.');
            if (dot != std::string::npos) name = name.substr(0, dot);
            return "\\Driver\\" + name;
        }
    }
    return "\\Driver\\Unknown";
}

void allocate_driver_object(CPU* cpu, PE* driver_pe) {
    const uint8_t ret_stub = 0xC3;

    if (!cpu->mem_map(STRUCT_BASE, 0x1000, PROT_READ | PROT_WRITE | PROT_EXEC))
        Debug::debug_msg("failed to map STRUCT_BASE", LOG_ERROR);

    mw(cpu, STRUCT_BASE + OFF_DEFDISP, &ret_stub, sizeof(ret_stub));

    const uint64_t default_dispatch = STRUCT_BASE + OFF_DEFDISP;

    auto drv_name_buf = to_utf16(pe_module_name(driver_pe));
    const USHORT   drv_name_len    = static_cast<USHORT>((drv_name_buf.size() - 1) * sizeof(uint16_t));
    const USHORT   drv_name_maxlen = static_cast<USHORT>(drv_name_buf.size() * sizeof(uint16_t));
    const uint64_t drv_name_addr   = STRUCT_BASE + OFF_DRVNAME;
    mw(cpu, drv_name_addr, drv_name_buf.data(), drv_name_buf.size() * sizeof(uint16_t));

    const uint16_t hw_db[] = {
        '\\','R','e','g','i','s','t','r','y','\\',
        'M','a','c','h','i','n','e','\\',
        'H','a','r','d','w','a','r','e','\\',
        'D','e','s','c','r','i','p','t','i','o','n','\\',
        'S','y','s','t','e','m', 0
    };
    const uint64_t hw_db_addr = STRUCT_BASE + OFF_HWDB;
    mw(cpu, hw_db_addr, hw_db, sizeof(hw_db));

    UNICODE_STRING hw_db_us;
    hw_db_us.Length        = static_cast<USHORT>(sizeof(hw_db) - 2);
    hw_db_us.MaximumLength = static_cast<USHORT>(sizeof(hw_db));
    hw_db_us.Buffer        = reinterpret_cast<uint16_t*>(hw_db_addr);
    const uint64_t hw_db_us_addr = hw_db_addr + sizeof(hw_db);
    mw(cpu, hw_db_us_addr, &hw_db_us, sizeof(UNICODE_STRING));

    _DRIVER_EXTENSION drvext{};
    drvext.DriverObject   = reinterpret_cast<PDRIVER_OBJECT>(STRUCT_BASE + OFF_DRVOBJ);
    drvext.AddDevice      = nullptr;
    drvext.Count          = 0;
    drvext.ServiceKeyName = {};
    mw(cpu, STRUCT_BASE + OFF_DRVEXT, &drvext, sizeof(_DRIVER_EXTENSION));

    DRIVER_OBJECT dro{};
    dro.Type                     = 4;
    dro.Size                     = sizeof(DRIVER_OBJECT);
    dro.DeviceObject             = nullptr;
    dro.Flags                    = DRVO_LEGACY_DRIVER | DRVO_INITIALIZED;
    dro.DriverStart              = reinterpret_cast<PVOID>(CODE_BASE);
    dro.DriverSize               = driver_pe->image_optional_header.SizeOfImage;
    dro.DriverSection            = nullptr;
    dro.DriverExtension          = reinterpret_cast<_DRIVER_EXTENSION*>(STRUCT_BASE + OFF_DRVEXT);
    dro.DriverName.Length        = drv_name_len;
    dro.DriverName.MaximumLength = drv_name_maxlen;
    dro.DriverName.Buffer        = reinterpret_cast<uint16_t*>(drv_name_addr);
    dro.HardwareDatabase         = reinterpret_cast<UNICODE_STRING*>(hw_db_us_addr);
    dro.FastIoDispatch           = nullptr;
    dro.DriverInit               = reinterpret_cast<PDRIVER_INITIALIZE>(
                                       CODE_BASE + driver_pe->image_optional_header.AddressOfEntryPoint);
    dro.DriverStartIo            = nullptr;
    dro.DriverUnload             = nullptr;

    for (auto& fn : dro.MajorFunction)
        fn = reinterpret_cast<PDRIVER_DISPATCH>(default_dispatch);

    mw(cpu, STRUCT_BASE + OFF_DRVOBJ, &dro, sizeof(DRIVER_OBJECT));
}

void allocate_device_object(CPU* cpu) {
    if (!cpu->mem_map(DEVICE_BASE, 0x1000, PROT_READ | PROT_WRITE))
        Debug::debug_msg("failed to map DEVICE_BASE", LOG_ERROR);

    DEVICE_OBJECT dev{};
    dev.Type                  = 3;
    dev.Size                  = sizeof(DEVICE_OBJECT);
    dev.ReferenceCount        = 1;
    dev.DriverObject          = reinterpret_cast<PDRIVER_OBJECT>(STRUCT_BASE + OFF_DRVOBJ);
    dev.NextDevice            = nullptr;
    dev.AttachedDevice        = nullptr;
    dev.CurrentIrp            = nullptr;
    dev.Flags                 = DO_BUFFERED_IO;
    dev.Characteristics       = 0;
    dev.Vpb                   = nullptr;
    dev.DeviceExtension       = reinterpret_cast<PVOID>(DEVICE_BASE + OFF_DEVEXT);
    dev.DeviceType            = FILE_DEVICE_UNKNOWN;
    dev.StackSize             = 1;
    dev.AlignmentRequirement  = FILE_BYTE_ALIGNMENT;
    dev.SectorSize            = 0;
    dev.DeviceObjectExtension = nullptr;

    mw(cpu, DEVICE_BASE + OFF_DEVOBJ, &dev, sizeof(DEVICE_OBJECT));

    const uint64_t dev_ptr = DEVICE_BASE + OFF_DEVOBJ;
    mw(cpu, STRUCT_BASE + OFF_DRVOBJ + offsetof(DRIVER_OBJECT, DeviceObject),
       &dev_ptr, sizeof(uint64_t));
}

void allocate_kuser(CPU* cpu) {
    size_t map_size = align_up_4k(sizeof(KUSER_SHARED_DATA)); 

    if (!cpu->mem_map(KUSER_BASE, map_size, PROT_READ | PROT_WRITE))
        Debug::debug_msg("failed to map KUSER_BASE", LOG_ERROR);

    KUSER_SHARED_DATA kus{};
    kus.TickCountMultiplier         = 0x0FA00000;
    kus.NtBuildNumber               = 22621;
    kus.NtMajorVersion              = 10;
    kus.NtMinorVersion              = 0;
    kus.NtProductType               = NtProductWinNt;
    kus.ProductTypeIsValid          = 1;
    kus.KdDebuggerEnabled           = 0;
    kus.SafeBootMode                = 0;
    kus.ActiveProcessorCount        = 1;
    kus.ActiveGroupCount            = 1;
    kus.QpcFrequency                = 10000000;
    kus.QpcBypassEnabled            = 1;
    kus.SuiteMask                   = 0x00000110;
    kus.NativeProcessorArchitecture = 9;

    kus.ProcessorFeatures[2]  = 1;
    kus.ProcessorFeatures[3]  = 1;
    kus.ProcessorFeatures[6]  = 1;
    kus.ProcessorFeatures[8]  = 1;
    kus.ProcessorFeatures[10] = 1;
    kus.ProcessorFeatures[12] = 1;
    kus.ProcessorFeatures[13] = 1;
    kus.ProcessorFeatures[14] = 1;
    kus.ProcessorFeatures[22] = 1;
    kus.ProcessorFeatures[23] = 1;
    kus.ProcessorFeatures[26] = 1;

    mw(cpu, KUSER_BASE, &kus, sizeof(KUSER_SHARED_DATA));
}