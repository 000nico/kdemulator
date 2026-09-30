#pragma once
#include "../Api.hpp"
#include "../emu-core/src/kernel/include/structs.hpp"
#include "../emu-core/src/memory/layout.hpp"
#include "../emu-core/src/cpu/perms.hpp"
#include "../../../emu-core/src/debug/Debug.hpp"
#include "../../../sdk/hex.hpp"

class ApiIoCreateDevice : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t driver_object_addr     = get_arg(cpu, 0);
        uint32_t device_extension_size  = static_cast<uint32_t>(get_arg(cpu, 1));
        uint64_t device_name_ptr        = get_arg(cpu, 2);
        uint32_t device_type            = static_cast<uint32_t>(get_arg(cpu, 3));
        uint32_t device_characteristics = static_cast<uint32_t>(get_arg(cpu, 4));
        uint8_t  exclusive              = static_cast<uint8_t>(get_arg(cpu, 5));
        uint64_t device_object_out      = get_arg(cpu, 6);

        uint32_t total = sizeof(DEVICE_OBJECT) + device_extension_size;
        cpu->mem_map(DEVICE_BASE, (total + 0xFFF) & ~0xFFF, PROT_READ | PROT_WRITE);

        std::string dev_name = read_unicode_string(cpu, device_name_ptr);
        Debug::debug_msg("[IoCreateDevice] drv=" + hex64(driver_object_addr) +
                         " name=\"" + dev_name + "\" extSize=" + std::to_string(device_extension_size), LOG_INFO);

        uint64_t devobj_addr = DEVICE_BASE + OFF_DEVOBJ; 
        uint64_t devext_addr = DEVICE_BASE + OFF_DEVEXT;

        DEVICE_OBJECT dev{};
        dev.Type                  = 3;
        dev.Size                  = static_cast<USHORT>(sizeof(DEVICE_OBJECT) + device_extension_size);
        dev.ReferenceCount        = 1;
        dev.DriverObject          = reinterpret_cast<PDRIVER_OBJECT>(driver_object_addr);
        dev.NextDevice            = nullptr;
        dev.AttachedDevice        = nullptr;
        dev.CurrentIrp            = nullptr;
        dev.Flags                 = exclusive ? DO_EXCLUSIVE : 0;
        dev.Characteristics       = device_characteristics;
        dev.Vpb                   = nullptr;
        dev.DeviceExtension       = device_extension_size? reinterpret_cast<PVOID>(devext_addr) : nullptr;
        dev.DeviceType            = device_type;
        dev.StackSize             = 1;
        dev.AlignmentRequirement  = FILE_BYTE_ALIGNMENT;
        dev.SectorSize            = 0;
        dev.DeviceObjectExtension = nullptr;

        cpu->mem_write(devobj_addr, reinterpret_cast<void*>(&dev), sizeof(DEVICE_OBJECT));
        cpu->mem_write(device_object_out, reinterpret_cast<void*>(&devobj_addr), sizeof(uint64_t));
        cpu->mem_write(driver_object_addr + offsetof(DRIVER_OBJECT, DeviceObject), reinterpret_cast<void*>(&devobj_addr), sizeof(uint64_t));

        return 0;
    }
};