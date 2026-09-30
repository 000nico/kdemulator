#pragma once
#include "../Api.hpp"
#include "../memory/allocators_common.hpp"
#include "../ps/ProcessThread.hpp"
#include "../../../emu-core/src/debug/Debug.hpp"
#include "../../../sdk/hex.hpp"
#include <string>

// IoCreateSymbolicLink(PUNICODE_STRING SymbolicLinkName, PUNICODE_STRING DeviceName) -> NTSTATUS
class ApiIoCreateSymbolicLink : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t symlink_addr = get_arg(cpu, 0);
        uint64_t device_addr  = get_arg(cpu, 1);

        std::string sym = read_unicode_string(cpu, symlink_addr);
        std::string dev = read_unicode_string(cpu, device_addr);

        Debug::debug_msg("[IoCreateSymbolicLink] \"" + sym + "\" -> \"" + dev + "\"", LOG_INFO);
        return 0; // STATUS_SUCCESS
    }
};

// IoDeleteSymbolicLink(PUNICODE_STRING SymbolicLinkName) -> NTSTATUS
class ApiIoDeleteSymbolicLink : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t symlink_addr = get_arg(cpu, 0);
        std::string sym = read_unicode_string(cpu, symlink_addr);

        Debug::debug_msg("[IoDeleteSymbolicLink] \"" + sym + "\"", LOG_INFO);
        return 0; // STATUS_SUCCESS
    }
};

// IoDeleteDevice(PDEVICE_OBJECT DeviceObject)
class ApiIoDeleteDevice : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t dev = get_arg(cpu, 0);
        Debug::debug_msg("[IoDeleteDevice] dev=" + hex64(dev), LOG_INFO);
        return 0;
    }
};

// IoCompleteRequest(PIRP Irp, CCHAR PriorityBoost) / IofCompleteRequest
class ApiIoCompleteRequest : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t irp   = get_arg(cpu, 0);
        uint8_t boost = static_cast<uint8_t>(get_arg(cpu, 1));
        Debug::debug_msg("[IoCompleteRequest] irp=" + hex64(irp) + " boost=" + std::to_string(boost), LOG_INFO);
        return 0;
    }
};

// IoAttachDevice(PDEVICE_OBJECT SourceDevice, PUNICODE_STRING TargetDevice, PDEVICE_OBJECT* AttachedDevice)
class ApiIoAttachDevice : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t src_dev     = get_arg(cpu, 0);
        uint64_t target_name = get_arg(cpu, 1);
        uint64_t out_dev     = get_arg(cpu, 2);

        std::string target = read_unicode_string(cpu, target_name);
        if (out_dev != 0) {
            write_u64(cpu, out_dev, src_dev);
        }

        Debug::debug_msg("[IoAttachDevice] src=" + hex64(src_dev) + " target=\"" + target + "\"", LOG_INFO);
        return 0; // STATUS_SUCCESS
    }
};

// IoAttachDeviceToDeviceStack(PDEVICE_OBJECT SourceDevice, PDEVICE_OBJECT TargetDevice) -> PDEVICE_OBJECT
class ApiIoAttachDeviceToDeviceStack : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t src_dev    = get_arg(cpu, 0);
        uint64_t target_dev = get_arg(cpu, 1);

        Debug::debug_msg("[IoAttachDeviceToDeviceStack] src=" + hex64(src_dev) +
                         " -> target=" + hex64(target_dev), LOG_INFO);
        return target_dev;
    }
};

// IoDetachDevice(PDEVICE_OBJECT TargetDevice)
class ApiIoDetachDevice : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t target_dev = get_arg(cpu, 0);
        Debug::debug_msg("[IoDetachDevice] target=" + hex64(target_dev), LOG_INFO);
        return 0;
    }
};

// IoGetDeviceObjectPointer(PUNICODE_STRING DeviceName, ACCESS_MASK DesiredAccess, PFILE_OBJECT* FileObject, PDEVICE_OBJECT* DeviceObject)
class ApiIoGetDeviceObjectPointer : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t dev_name_addr = get_arg(cpu, 0);
        uint32_t access        = static_cast<uint32_t>(get_arg(cpu, 1));
        uint64_t file_obj_out  = get_arg(cpu, 2);
        uint64_t dev_obj_out   = get_arg(cpu, 3);

        std::string name = read_unicode_string(cpu, dev_name_addr);

        static uint64_t s_file_obj = 0;
        static uint64_t s_dev_obj  = 0;
        if (s_file_obj == 0) s_file_obj = allocate(0x200, 0, 0x6C69466F, cpu); // 'oFil'
        if (s_dev_obj == 0)  s_dev_obj  = allocate(0x200, 0, 0x7665446F, cpu); // 'oDev'

        if (file_obj_out != 0) write_u64(cpu, file_obj_out, s_file_obj);
        if (dev_obj_out != 0)  write_u64(cpu, dev_obj_out, s_dev_obj);

        Debug::debug_msg("[IoGetDeviceObjectPointer] name=\"" + name + "\" access=" + hex32(access), LOG_INFO);
        return 0; // STATUS_SUCCESS
    }
};

// IoRegisterShutdownNotification(PDEVICE_OBJECT DeviceObject)
class ApiIoRegisterShutdownNotification : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t dev = get_arg(cpu, 0);
        Debug::debug_msg("[IoRegisterShutdownNotification] dev=" + hex64(dev), LOG_INFO);
        return 0; // STATUS_SUCCESS
    }
};

// IoUnregisterShutdownNotification(PDEVICE_OBJECT DeviceObject)
class ApiIoUnregisterShutdownNotification : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t dev = get_arg(cpu, 0);
        Debug::debug_msg("[IoUnregisterShutdownNotification] dev=" + hex64(dev), LOG_INFO);
        return 0; // STATUS_SUCCESS
    }
};

// IoRegisterDriverReinitialization(PDRIVER_OBJECT DriverObject, PDRIVER_REINITIALIZE DriverReinitializationRoutine, PVOID Context)
class ApiIoRegisterDriverReinitialization : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t drv     = get_arg(cpu, 0);
        uint64_t routine = get_arg(cpu, 1);
        Debug::debug_msg("[IoRegisterDriverReinitialization] drv=" + hex64(drv) + " routine=" + hex64(routine), LOG_INFO);
        return 0;
    }
};

// IoAllocateIrp(CCHAR StackSize, BOOLEAN ChargeQuota) -> PIRP
class ApiIoAllocateIrp : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint8_t stack_size   = static_cast<uint8_t>(get_arg(cpu, 0));
        uint8_t charge_quota = static_cast<uint8_t>(get_arg(cpu, 1));

        uint64_t irp_addr = allocate(0x100, 0, 0x20707249, cpu); // 'Irp '
        Debug::debug_msg("[IoAllocateIrp] stackSize=" + std::to_string(stack_size) +
                         " -> irp=" + hex64(irp_addr), LOG_INFO);
        return irp_addr;
    }
};

// IoFreeIrp(PIRP Irp)
class ApiIoFreeIrp : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t irp = get_arg(cpu, 0);
        Debug::debug_msg("[IoFreeIrp] irp=" + hex64(irp), LOG_INFO);
        return 0;
    }
};

// IoInitializeIrp(PIRP Irp, USHORT PacketSize, CCHAR StackSize)
class ApiIoInitializeIrp : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t irp   = get_arg(cpu, 0);
        uint16_t size  = static_cast<uint16_t>(get_arg(cpu, 1));
        uint8_t stacks = static_cast<uint8_t>(get_arg(cpu, 2));
        Debug::debug_msg("[IoInitializeIrp] irp=" + hex64(irp) + " size=" + std::to_string(size) +
                         " stacks=" + std::to_string(stacks), LOG_INFO);
        return 0;
    }
};

// IoCallDriver(PDEVICE_OBJECT DeviceObject, PIRP Irp) / IofCallDriver
class ApiIoCallDriver : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t dev = get_arg(cpu, 0);
        uint64_t irp = get_arg(cpu, 1);
        Debug::debug_msg("[IoCallDriver] dev=" + hex64(dev) + " irp=" + hex64(irp), LOG_INFO);
        return 0; // STATUS_SUCCESS
    }
};

// IoReuseIrp(PIRP Irp, NTSTATUS Iostatus)
class ApiIoReuseIrp : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t irp    = get_arg(cpu, 0);
        uint32_t status = static_cast<uint32_t>(get_arg(cpu, 1));
        Debug::debug_msg("[IoReuseIrp] irp=" + hex64(irp) + " status=" + hex32(status), LOG_INFO);
        return 0;
    }
};

// ObReferenceObjectByHandle(...) -> NTSTATUS
class ApiObReferenceObjectByHandle : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t handle      = get_arg(cpu, 0);
        uint32_t access      = static_cast<uint32_t>(get_arg(cpu, 1));
        uint64_t obj_type    = get_arg(cpu, 2);
        uint8_t  mode        = static_cast<uint8_t>(get_arg(cpu, 3));
        uint64_t out_obj     = get_arg(cpu, 4);
        uint64_t out_handle  = get_arg(cpu, 5);

        uint64_t target_obj = get_dummy_eprocess(cpu);
        if (out_obj != 0) {
            write_u64(cpu, out_obj, target_obj);
        }

        Debug::debug_msg("[ObReferenceObjectByHandle] handle=" + hex64(handle) +
                         " access=" + hex32(access) + " -> obj=" + hex64(target_obj), LOG_INFO);
        return 0; // STATUS_SUCCESS
    }
};

// ObReferenceObjectByName(...) -> NTSTATUS
class ApiObReferenceObjectByName : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t obj_name = get_arg(cpu, 0);
        uint64_t out_obj  = get_arg(cpu, 7);

        std::string name = read_unicode_string(cpu, obj_name);
        uint64_t target_obj = get_dummy_eprocess(cpu);
        if (out_obj != 0) {
            write_u64(cpu, out_obj, target_obj);
        }

        Debug::debug_msg("[ObReferenceObjectByName] name=\"" + name + "\" -> obj=" + hex64(target_obj), LOG_INFO);
        return 0; // STATUS_SUCCESS
    }
};

// ObReferenceObject(PVOID Object) -> LONG
class ApiObReferenceObject : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t obj = get_arg(cpu, 0);
        Debug::debug_msg("[ObReferenceObject] obj=" + hex64(obj), LOG_INFO);
        return 1;
    }
};

// ObfDereferenceObject(PVOID Object) / ObDereferenceObject
class ApiObfDereferenceObject : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t obj = get_arg(cpu, 0);
        Debug::debug_msg("[ObfDereferenceObject] obj=" + hex64(obj), LOG_INFO);
        return 0;
    }
};

// ObOpenObjectByPointer(PVOID Object, ULONG HandleAttributes, PACCESS_STATE PassedAccessState, ACCESS_MASK DesiredAccess, POBJECT_TYPE ObjectType, KPROCESSOR_MODE AccessMode, PHANDLE Handle)
class ApiObOpenObjectByPointer : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t obj        = get_arg(cpu, 0);
        uint32_t access     = static_cast<uint32_t>(get_arg(cpu, 3));
        uint64_t out_handle = get_arg(cpu, 6);

        uint64_t handle = 0x120;
        if (out_handle != 0) {
            write_u64(cpu, out_handle, handle);
        }

        Debug::debug_msg("[ObOpenObjectByPointer] obj=" + hex64(obj) + " -> handle=" + hex64(handle), LOG_INFO);
        return 0; // STATUS_SUCCESS
    }
};

// ObGetObjectType(PVOID Object) -> POBJECT_TYPE
class ApiObGetObjectType : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t obj = get_arg(cpu, 0);
        static uint64_t s_obj_type = 0;
        if (s_obj_type == 0) s_obj_type = allocate(0x100, 0, 0x65707954, cpu); // 'Type'

        Debug::debug_msg("[ObGetObjectType] obj=" + hex64(obj) + " -> type=" + hex64(s_obj_type), LOG_INFO);
        return s_obj_type;
    }
};

// ObQueryNameString(PVOID Object, POBJECT_NAME_INFORMATION ObjectNameInfo, ULONG Length, PULONG ReturnLength)
class ApiObQueryNameString : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t obj        = get_arg(cpu, 0);
        uint64_t info_addr  = get_arg(cpu, 1);
        uint32_t length     = static_cast<uint32_t>(get_arg(cpu, 2));
        uint64_t ret_len    = get_arg(cpu, 3);

        if (ret_len != 0) {
            write_u32(cpu, ret_len, 0x40);
        }
        Debug::debug_msg("[ObQueryNameString] obj=" + hex64(obj) + " len=" + std::to_string(length), LOG_INFO);
        return 0; // STATUS_SUCCESS
    }
};

// ApiObjectType for exported object type pointers (PsProcessType, PsThreadType, IoDriverObjectType, IoDeviceObjectType)
class ApiObjectType : public Api {
public:
    uint64_t call(CPU* cpu) override {
        static uint64_t s_type_obj = 0;
        if (s_type_obj == 0) s_type_obj = allocate(0x100, 0, 0x65707954, cpu); // 'Type'
        Debug::debug_msg("[ObjectType] -> " + hex64(s_type_obj), LOG_INFO);
        return s_type_obj;
    }
};

