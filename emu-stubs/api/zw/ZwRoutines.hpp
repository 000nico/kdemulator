#pragma once
#include "../Api.hpp"
#include "../memory/allocators_common.hpp"
#include "../../../emu-core/src/debug/Debug.hpp"
#include "../../../sdk/hex.hpp"
#include <string>
#include <vector>

// ZwOpenKey(PHANDLE KeyHandle, ACCESS_MASK DesiredAccess, POBJECT_ATTRIBUTES ObjectAttributes) -> NTSTATUS
class ApiZwOpenKey : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t key_handle_ptr = get_arg(cpu, 0);
        uint32_t access         = static_cast<uint32_t>(get_arg(cpu, 1));
        uint64_t obj_attr       = get_arg(cpu, 2);

        uint64_t handle = 0x200;
        if (key_handle_ptr != 0) {
            write_u64(cpu, key_handle_ptr, handle);
        }

        Debug::debug_msg("[ZwOpenKey] access=" + hex32(access) + " -> handle=" + hex64(handle), LOG_INFO);
        return 0; // STATUS_SUCCESS
    }
};

// ZwCreateKey(PHANDLE KeyHandle, ACCESS_MASK DesiredAccess, POBJECT_ATTRIBUTES ObjectAttributes, ULONG TitleIndex, PUNICODE_STRING Class, ULONG CreateOptions, PULONG Disposition)
class ApiZwCreateKey : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t key_handle_ptr = get_arg(cpu, 0);
        uint32_t access         = static_cast<uint32_t>(get_arg(cpu, 1));
        uint64_t disp_ptr       = get_arg(cpu, 6);

        uint64_t handle = 0x204;
        if (key_handle_ptr != 0) {
            write_u64(cpu, key_handle_ptr, handle);
        }
        if (disp_ptr != 0) {
            write_u32(cpu, disp_ptr, 1); // REG_CREATED_NEW_KEY
        }

        Debug::debug_msg("[ZwCreateKey] access=" + hex32(access) + " -> handle=" + hex64(handle), LOG_INFO);
        return 0; // STATUS_SUCCESS
    }
};

// ZwClose(HANDLE Handle) -> NTSTATUS
class ApiZwClose : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t handle = get_arg(cpu, 0);
        Debug::debug_msg("[ZwClose] handle=" + hex64(handle), LOG_INFO);
        return 0; // STATUS_SUCCESS
    }
};

// ZwQueryValueKey(HANDLE KeyHandle, PUNICODE_STRING ValueName, KEY_VALUE_INFORMATION_CLASS KeyValueInformationClass, PVOID KeyValueInformation, ULONG Length, PULONG ResultLength)
class ApiZwQueryValueKey : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t handle      = get_arg(cpu, 0);
        uint64_t val_name    = get_arg(cpu, 1);
        uint32_t info_class  = static_cast<uint32_t>(get_arg(cpu, 2));
        uint64_t info_buf    = get_arg(cpu, 3);
        uint32_t length      = static_cast<uint32_t>(get_arg(cpu, 4));
        uint64_t res_len_ptr = get_arg(cpu, 5);

        std::string name = read_unicode_string(cpu, val_name);
        if (res_len_ptr != 0) {
            write_u32(cpu, res_len_ptr, 0x20);
        }

        Debug::debug_msg("[ZwQueryValueKey] handle=" + hex64(handle) + " name=\"" + name + "\"", LOG_INFO);
        return 0; // STATUS_SUCCESS
    }
};

// ZwSetValueKey(HANDLE KeyHandle, PUNICODE_STRING ValueName, ULONG TitleIndex, ULONG Type, PVOID Data, ULONG DataSize)
class ApiZwSetValueKey : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t handle   = get_arg(cpu, 0);
        uint64_t val_name = get_arg(cpu, 1);
        uint32_t type     = static_cast<uint32_t>(get_arg(cpu, 3));
        uint32_t size     = static_cast<uint32_t>(get_arg(cpu, 5));

        std::string name = read_unicode_string(cpu, val_name);
        Debug::debug_msg("[ZwSetValueKey] handle=" + hex64(handle) + " name=\"" + name + "\" size=" + std::to_string(size), LOG_INFO);
        return 0; // STATUS_SUCCESS
    }
};

// ZwDeleteKey(HANDLE KeyHandle) -> NTSTATUS
class ApiZwDeleteKey : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t handle = get_arg(cpu, 0);
        Debug::debug_msg("[ZwDeleteKey] handle=" + hex64(handle), LOG_INFO);
        return 0;
    }
};

// ZwDeleteValueKey(HANDLE KeyHandle, PUNICODE_STRING ValueName) -> NTSTATUS
class ApiZwDeleteValueKey : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t handle   = get_arg(cpu, 0);
        uint64_t val_name = get_arg(cpu, 1);
        std::string name = read_unicode_string(cpu, val_name);
        Debug::debug_msg("[ZwDeleteValueKey] handle=" + hex64(handle) + " name=\"" + name + "\"", LOG_INFO);
        return 0;
    }
};

// ZwEnumerateKey(...) -> NTSTATUS
class ApiZwEnumerateKey : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t handle = get_arg(cpu, 0);
        uint32_t index  = static_cast<uint32_t>(get_arg(cpu, 1));
        Debug::debug_msg("[ZwEnumerateKey] handle=" + hex64(handle) + " index=" + std::to_string(index), LOG_INFO);
        return 0x8000001A; // STATUS_NO_MORE_ENTRIES
    }
};

// ZwEnumerateValueKey(...) -> NTSTATUS
class ApiZwEnumerateValueKey : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t handle = get_arg(cpu, 0);
        uint32_t index  = static_cast<uint32_t>(get_arg(cpu, 1));
        Debug::debug_msg("[ZwEnumerateValueKey] handle=" + hex64(handle) + " index=" + std::to_string(index), LOG_INFO);
        return 0x8000001A; // STATUS_NO_MORE_ENTRIES
    }
};

// RtlQueryRegistryValues(...) -> NTSTATUS
class ApiRtlQueryRegistryValues : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint32_t relative_to = static_cast<uint32_t>(get_arg(cpu, 0));
        uint64_t path_addr   = get_arg(cpu, 1);
        std::wstring path = read_wide_string(cpu, path_addr);
        std::string path_a(path.begin(), path.end());
        Debug::debug_msg("[RtlQueryRegistryValues] relativeTo=" + std::to_string(relative_to) +
                         " path=\"" + path_a + "\"", LOG_INFO);
        return 0; // STATUS_SUCCESS
    }
};

// ZwOpenFile(PHANDLE FileHandle, ACCESS_MASK DesiredAccess, POBJECT_ATTRIBUTES ObjectAttributes, PIO_STATUS_BLOCK IoStatusBlock, ULONG ShareAccess, ULONG OpenOptions)
class ApiZwOpenFile : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t handle_ptr = get_arg(cpu, 0);
        uint32_t access     = static_cast<uint32_t>(get_arg(cpu, 1));
        uint64_t iosb       = get_arg(cpu, 3);

        uint64_t handle = 0x400;
        if (handle_ptr != 0) write_u64(cpu, handle_ptr, handle);
        if (iosb != 0) {
            write_u64(cpu, iosb, 0); // Status = STATUS_SUCCESS
            write_u64(cpu, iosb + 8, 1); // Information = FILE_OPENED
        }

        Debug::debug_msg("[ZwOpenFile] access=" + hex32(access) + " -> handle=" + hex64(handle), LOG_INFO);
        return 0; // STATUS_SUCCESS
    }
};

// ZwCreateFile(PHANDLE FileHandle, ...)
class ApiZwCreateFile : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t handle_ptr = get_arg(cpu, 0);
        uint32_t access     = static_cast<uint32_t>(get_arg(cpu, 1));
        uint64_t iosb       = get_arg(cpu, 4);

        uint64_t handle = 0x404;
        if (handle_ptr != 0) write_u64(cpu, handle_ptr, handle);
        if (iosb != 0) {
            write_u64(cpu, iosb, 0);
            write_u64(cpu, iosb + 8, 2); // Information = FILE_CREATED
        }

        Debug::debug_msg("[ZwCreateFile] access=" + hex32(access) + " -> handle=" + hex64(handle), LOG_INFO);
        return 0; // STATUS_SUCCESS
    }
};

// ZwReadFile(...) -> NTSTATUS
class ApiZwReadFile : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t handle = get_arg(cpu, 0);
        uint64_t iosb   = get_arg(cpu, 4);
        uint32_t length = static_cast<uint32_t>(get_arg(cpu, 6));

        if (iosb != 0) {
            write_u64(cpu, iosb, 0);
            write_u64(cpu, iosb + 8, length);
        }

        Debug::debug_msg("[ZwReadFile] handle=" + hex64(handle) + " len=" + std::to_string(length), LOG_INFO);
        return 0; // STATUS_SUCCESS
    }
};

// ZwWriteFile(...) -> NTSTATUS
class ApiZwWriteFile : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t handle = get_arg(cpu, 0);
        uint64_t iosb   = get_arg(cpu, 4);
        uint32_t length = static_cast<uint32_t>(get_arg(cpu, 6));

        if (iosb != 0) {
            write_u64(cpu, iosb, 0);
            write_u64(cpu, iosb + 8, length);
        }

        Debug::debug_msg("[ZwWriteFile] handle=" + hex64(handle) + " len=" + std::to_string(length), LOG_INFO);
        return 0; // STATUS_SUCCESS
    }
};

// ZwQueryInformationFile(...) -> NTSTATUS
class ApiZwQueryInformationFile : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t handle     = get_arg(cpu, 0);
        uint64_t iosb       = get_arg(cpu, 1);
        uint32_t info_class = static_cast<uint32_t>(get_arg(cpu, 4));

        if (iosb != 0) {
            write_u64(cpu, iosb, 0);
        }

        Debug::debug_msg("[ZwQueryInformationFile] handle=" + hex64(handle) + " class=" + std::to_string(info_class), LOG_INFO);
        return 0; // STATUS_SUCCESS
    }
};

// ZwSetInformationFile(...) -> NTSTATUS
class ApiZwSetInformationFile : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t handle     = get_arg(cpu, 0);
        uint64_t iosb       = get_arg(cpu, 1);
        uint32_t info_class = static_cast<uint32_t>(get_arg(cpu, 4));

        if (iosb != 0) {
            write_u64(cpu, iosb, 0);
        }

        Debug::debug_msg("[ZwSetInformationFile] handle=" + hex64(handle) + " class=" + std::to_string(info_class), LOG_INFO);
        return 0; // STATUS_SUCCESS
    }
};

// ZwQuerySystemInformation(SYSTEM_INFORMATION_CLASS SystemInformationClass, PVOID SystemInformation, ULONG SystemInformationLength, PULONG ReturnLength)
class ApiZwQuerySystemInformation : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint32_t info_class = static_cast<uint32_t>(get_arg(cpu, 0));
        uint64_t buf        = get_arg(cpu, 1);
        uint32_t len        = static_cast<uint32_t>(get_arg(cpu, 2));
        uint64_t ret_len    = get_arg(cpu, 3);

        if (buf != 0 && len > 0) {
            std::vector<uint8_t> zeros(len, 0);
            cpu->mem_write(buf, zeros.data(), len);
        }
        if (ret_len != 0) {
            write_u32(cpu, ret_len, len > 0 ? len : 0x100);
        }

        Debug::debug_msg("[ZwQuerySystemInformation] class=" + hex32(info_class) +
                         " len=" + std::to_string(len), LOG_INFO);
        return 0; // STATUS_SUCCESS
    }
};

// ZwQueryInformationProcess(HANDLE ProcessHandle, PROCESSINFOCLASS ProcessInformationClass, PVOID ProcessInformation, ULONG ProcessInformationLength, PULONG ReturnLength)
class ApiZwQueryInformationProcess : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t handle     = get_arg(cpu, 0);
        uint32_t info_class = static_cast<uint32_t>(get_arg(cpu, 1));
        uint64_t buf        = get_arg(cpu, 2);
        uint32_t len        = static_cast<uint32_t>(get_arg(cpu, 3));
        uint64_t ret_len    = get_arg(cpu, 4);

        if (buf != 0 && len > 0) {
            std::vector<uint8_t> zeros(len, 0);
            cpu->mem_write(buf, zeros.data(), len);
        }
        if (ret_len != 0) {
            write_u32(cpu, ret_len, len);
        }

        Debug::debug_msg("[ZwQueryInformationProcess] handle=" + hex64(handle) + " class=" + std::to_string(info_class), LOG_INFO);
        return 0; // STATUS_SUCCESS
    }
};

// ZwSetInformationProcess(...) -> NTSTATUS
class ApiZwSetInformationProcess : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t handle     = get_arg(cpu, 0);
        uint32_t info_class = static_cast<uint32_t>(get_arg(cpu, 1));
        Debug::debug_msg("[ZwSetInformationProcess] handle=" + hex64(handle) + " class=" + std::to_string(info_class), LOG_INFO);
        return 0; // STATUS_SUCCESS
    }
};

// ZwQueryInformationThread(HANDLE ThreadHandle, THREADINFOCLASS ThreadInformationClass, PVOID ThreadInformation, ULONG ThreadInformationLength, PULONG ReturnLength)
class ApiZwQueryInformationThread : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t handle     = get_arg(cpu, 0);
        uint32_t info_class = static_cast<uint32_t>(get_arg(cpu, 1));
        uint64_t buf        = get_arg(cpu, 2);
        uint32_t len        = static_cast<uint32_t>(get_arg(cpu, 3));
        uint64_t ret_len    = get_arg(cpu, 4);

        if (buf != 0 && len > 0) {
            std::vector<uint8_t> zeros(len, 0);
            cpu->mem_write(buf, zeros.data(), len);
        }
        if (ret_len != 0) {
            write_u32(cpu, ret_len, len);
        }

        Debug::debug_msg("[ZwQueryInformationThread] handle=" + hex64(handle) + " class=" + std::to_string(info_class), LOG_INFO);
        return 0; // STATUS_SUCCESS
    }
};

// ZwSetInformationThread(...) -> NTSTATUS
class ApiZwSetInformationThread : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t handle     = get_arg(cpu, 0);
        uint32_t info_class = static_cast<uint32_t>(get_arg(cpu, 1));
        Debug::debug_msg("[ZwSetInformationThread] handle=" + hex64(handle) + " class=" + std::to_string(info_class), LOG_INFO);
        return 0; // STATUS_SUCCESS
    }
};

// ZwOpenProcess(PHANDLE ProcessHandle, ACCESS_MASK DesiredAccess, POBJECT_ATTRIBUTES ObjectAttributes, PCLIENT_ID ClientId)
class ApiZwOpenProcess : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t handle_ptr = get_arg(cpu, 0);
        uint32_t access     = static_cast<uint32_t>(get_arg(cpu, 1));
        uint64_t client_id  = get_arg(cpu, 3);

        uint64_t handle = 0x300;
        if (handle_ptr != 0) write_u64(cpu, handle_ptr, handle);

        Debug::debug_msg("[ZwOpenProcess] access=" + hex32(access) + " -> handle=" + hex64(handle), LOG_INFO);
        return 0; // STATUS_SUCCESS
    }
};

// ZwOpenThread(PHANDLE ThreadHandle, ACCESS_MASK DesiredAccess, POBJECT_ATTRIBUTES ObjectAttributes, PCLIENT_ID ClientId)
class ApiZwOpenThread : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t handle_ptr = get_arg(cpu, 0);
        uint32_t access     = static_cast<uint32_t>(get_arg(cpu, 1));

        uint64_t handle = 0x304;
        if (handle_ptr != 0) write_u64(cpu, handle_ptr, handle);

        Debug::debug_msg("[ZwOpenThread] access=" + hex32(access) + " -> handle=" + hex64(handle), LOG_INFO);
        return 0; // STATUS_SUCCESS
    }
};

// ZwTerminateProcess(HANDLE ProcessHandle, NTSTATUS ExitStatus)
class ApiZwTerminateProcess : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t handle = get_arg(cpu, 0);
        uint32_t status = static_cast<uint32_t>(get_arg(cpu, 1));
        Debug::debug_msg("[ZwTerminateProcess] handle=" + hex64(handle) + " status=" + hex32(status), LOG_INFO);
        return 0; // STATUS_SUCCESS
    }
};

// ZwAllocateVirtualMemory(HANDLE ProcessHandle, PVOID* BaseAddress, ULONG_PTR ZeroBits, PSIZE_T RegionSize, ULONG AllocationType, ULONG Protect)
class ApiZwAllocateVirtualMemory : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t handle      = get_arg(cpu, 0);
        uint64_t base_ptr    = get_arg(cpu, 1);
        uint64_t size_ptr    = get_arg(cpu, 3);
        uint32_t alloc_type  = static_cast<uint32_t>(get_arg(cpu, 4));
        uint32_t protect     = static_cast<uint32_t>(get_arg(cpu, 5));

        size_t size = size_ptr ? static_cast<size_t>(read_u64(cpu, size_ptr)) : 0x1000;
        if (size == 0) size = 0x1000;

        uint64_t va = allocate(size, 0, 0x20204D56, cpu); // 'VM  '
        if (base_ptr != 0) write_u64(cpu, base_ptr, va);
        if (size_ptr != 0) write_u64(cpu, size_ptr, (size + 0xFFF) & ~0xFFFULL);

        Debug::debug_msg("[ZwAllocateVirtualMemory] size=" + std::to_string(size) + " -> va=" + hex64(va), LOG_INFO);
        return 0; // STATUS_SUCCESS
    }
};

// ZwFreeVirtualMemory(HANDLE ProcessHandle, PVOID* BaseAddress, PSIZE_T RegionSize, ULONG FreeType)
class ApiZwFreeVirtualMemory : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t handle   = get_arg(cpu, 0);
        uint64_t base_ptr = get_arg(cpu, 1);
        uint64_t va = base_ptr ? read_u64(cpu, base_ptr) : 0;
        Debug::debug_msg("[ZwFreeVirtualMemory] va=" + hex64(va), LOG_INFO);
        return 0; // STATUS_SUCCESS
    }
};

// ZwProtectVirtualMemory(HANDLE ProcessHandle, PVOID* BaseAddress, PSIZE_T RegionSize, ULONG NewProtect, PULONG OldProtect)
class ApiZwProtectVirtualMemory : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t base_ptr = get_arg(cpu, 1);
        uint64_t size_ptr = get_arg(cpu, 2);
        uint32_t new_prot = static_cast<uint32_t>(get_arg(cpu, 3));
        uint64_t old_ptr  = get_arg(cpu, 4);

        if (old_ptr != 0) write_u32(cpu, old_ptr, 0x40); // PAGE_EXECUTE_READWRITE

        Debug::debug_msg("[ZwProtectVirtualMemory] newProtect=" + hex32(new_prot), LOG_INFO);
        return 0; // STATUS_SUCCESS
    }
};

// ZwReadVirtualMemory(HANDLE ProcessHandle, PVOID BaseAddress, PVOID Buffer, SIZE_T BufferSize, PSIZE_T NumberOfBytesRead)
class ApiZwReadVirtualMemory : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t handle   = get_arg(cpu, 0);
        uint64_t base_va  = get_arg(cpu, 1);
        uint64_t buf      = get_arg(cpu, 2);
        size_t   size     = static_cast<size_t>(get_arg(cpu, 3));
        uint64_t read_ptr = get_arg(cpu, 4);

        std::vector<uint8_t> tmp(size);
        bool ok = cpu->mem_read(base_va, tmp.data(), size);
        if (ok) {
            cpu->mem_write(buf, tmp.data(), size);
        }
        if (read_ptr != 0) {
            write_u64(cpu, read_ptr, ok ? size : 0);
        }

        Debug::debug_msg("[ZwReadVirtualMemory] va=" + hex64(base_va) + " size=" + std::to_string(size), LOG_INFO);
        return ok ? 0 : 0xC0000005;
    }
};

// ZwWriteVirtualMemory(HANDLE ProcessHandle, PVOID BaseAddress, PVOID Buffer, SIZE_T BufferSize, PSIZE_T NumberOfBytesWritten)
class ApiZwWriteVirtualMemory : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t handle    = get_arg(cpu, 0);
        uint64_t base_va   = get_arg(cpu, 1);
        uint64_t buf       = get_arg(cpu, 2);
        size_t   size      = static_cast<size_t>(get_arg(cpu, 3));
        uint64_t write_ptr = get_arg(cpu, 4);

        std::vector<uint8_t> tmp(size);
        bool ok = cpu->mem_read(buf, tmp.data(), size);
        if (ok) {
            ok = cpu->mem_write(base_va, tmp.data(), size);
        }
        if (write_ptr != 0) {
            write_u64(cpu, write_ptr, ok ? size : 0);
        }

        Debug::debug_msg("[ZwWriteVirtualMemory] va=" + hex64(base_va) + " size=" + std::to_string(size), LOG_INFO);
        return ok ? 0 : 0xC0000005;
    }
};

// ExGetFirmwareEnvironmentVariable(PUNICODE_STRING VariableName, LPGUID VendorGuid, PVOID Value, PULONG ValueLength, PULONG Attributes) -> NTSTATUS
class ApiExGetFirmwareEnvironmentVariable : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t var_name_ptr = get_arg(cpu, 0);
        uint64_t val_buf      = get_arg(cpu, 2);
        uint64_t val_len_ptr  = get_arg(cpu, 3);
        uint64_t attr_ptr     = get_arg(cpu, 4);

        std::string name = read_unicode_string(cpu, var_name_ptr);
        // Frequently queried for SecureBoot (e.g. by anti-cheats)
        if (val_buf != 0) {
            uint8_t val = 1; // SecureBoot enabled
            cpu->mem_write(val_buf, &val, 1);
        }
        if (val_len_ptr != 0) {
            write_u32(cpu, val_len_ptr, 1);
        }
        if (attr_ptr != 0) {
            write_u32(cpu, attr_ptr, 7);
        }

        Debug::debug_msg("[ExGetFirmwareEnvironmentVariable] var=\"" + name + "\" -> 1", LOG_INFO);
        return 0; // STATUS_SUCCESS
    }
};

// ZwDeviceIoControlFile(...) -> NTSTATUS
class ApiZwDeviceIoControlFile : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t handle  = get_arg(cpu, 0);
        uint64_t iosb    = get_arg(cpu, 4);
        uint32_t ioctl   = static_cast<uint32_t>(get_arg(cpu, 5));
        uint32_t out_len = static_cast<uint32_t>(get_arg(cpu, 9));

        if (iosb != 0) {
            write_u64(cpu, iosb, 0); // STATUS_SUCCESS
            write_u64(cpu, iosb + 8, out_len); // Information
        }

        Debug::debug_msg("[ZwDeviceIoControlFile] handle=" + hex64(handle) + " ioctl=" + hex32(ioctl), LOG_INFO);
        return 0; // STATUS_SUCCESS
    }
};

