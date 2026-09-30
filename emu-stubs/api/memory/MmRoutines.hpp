#pragma once
#include "../Api.hpp"
#include "allocators_common.hpp"
#include "../../../emu-core/src/debug/Debug.hpp"
#include "../../../emu-core/src/memory/layout.hpp"
#include "../../../sdk/hex.hpp"
#include <string>
#include <vector>

// MDL minimal memory layout for emulation
#pragma pack(push, 4)
struct EMU_MDL {
    uint64_t Next;             // +0x00
    uint16_t Size;             // +0x08
    uint16_t MdlFlags;         // +0x0A
    uint32_t Pad0;             // +0x0C
    uint64_t Process;          // +0x10
    uint64_t MappedSystemVa;   // +0x18
    uint64_t StartVa;          // +0x20
    uint32_t ByteCount;        // +0x28
    uint32_t ByteOffset;       // +0x2C
};
#pragma pack(pop)

// MmIsAddressValid(PVOID VirtualAddress) -> BOOLEAN
class ApiMmIsAddressValid : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t addr = get_arg(cpu, 0);
        uint8_t byte_val = 0;
        bool valid = cpu->mem_read(addr, &byte_val, 1);
        Debug::debug_msg("[MmIsAddressValid] addr=" + hex64(addr) +
                         " -> " + (valid ? "TRUE" : "FALSE"), LOG_INFO);
        return valid ? 1 : 0;
    }
};

// MmCopyVirtualMemory(PEPROCESS SourceProcess, PVOID SourceAddress, PEPROCESS TargetProcess, PVOID TargetAddress, SIZE_T BufferSize, KPROCESSOR_MODE PreviousMode, PSIZE_T NumberOfBytesCopied)
class ApiMmCopyVirtualMemory : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t src_proc = get_arg(cpu, 0);
        uint64_t src_addr = get_arg(cpu, 1);
        uint64_t dst_proc = get_arg(cpu, 2);
        uint64_t dst_addr = get_arg(cpu, 3);
        size_t   size     = static_cast<size_t>(get_arg(cpu, 4));
        uint8_t  mode     = static_cast<uint8_t>(get_arg(cpu, 5));
        uint64_t out_copied = get_arg(cpu, 6);

        std::vector<uint8_t> buffer(size);
        bool read_ok = cpu->mem_read(src_addr, buffer.data(), size);
        bool write_ok = false;
        if (read_ok) {
            write_ok = cpu->mem_write(dst_addr, buffer.data(), size);
        }

        if (out_copied != 0) {
            write_u64(cpu, out_copied, write_ok ? size : 0);
        }

        Debug::debug_msg("[MmCopyVirtualMemory] " + hex64(src_addr) + " -> " + hex64(dst_addr) +
                         " size=" + std::to_string(size) + " status=" + (write_ok ? "OK" : "FAIL"), LOG_INFO);
        return write_ok ? 0 : 0xC0000005; // STATUS_ACCESS_VIOLATION on failure
    }
};

// MmMapIoSpace(PHYSICAL_ADDRESS PhysicalAddress, SIZE_T NumberOfBytes, MEMORY_CACHING_TYPE CacheType) -> PVOID
class ApiMmMapIoSpace : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t phys_addr = get_arg(cpu, 0);
        size_t   size      = static_cast<size_t>(get_arg(cpu, 1));
        uint32_t cache     = static_cast<uint32_t>(get_arg(cpu, 2));

        uint64_t va = allocate(size, 0, 0x4F49704D, cpu); // 'MpIO'
        Debug::debug_msg("[MmMapIoSpace] phys=" + hex64(phys_addr) + " size=" + std::to_string(size) +
                         " -> va=" + hex64(va), LOG_INFO);
        return va;
    }
};

// MmMapIoSpaceEx(PHYSICAL_ADDRESS PhysicalAddress, SIZE_T NumberOfBytes, ULONG Protect) -> PVOID
class ApiMmMapIoSpaceEx : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t phys_addr = get_arg(cpu, 0);
        size_t   size      = static_cast<size_t>(get_arg(cpu, 1));
        uint32_t protect   = static_cast<uint32_t>(get_arg(cpu, 2));

        uint64_t va = allocate(size, 0, 0x4F49704D, cpu);
        Debug::debug_msg("[MmMapIoSpaceEx] phys=" + hex64(phys_addr) + " size=" + std::to_string(size) +
                         " -> va=" + hex64(va), LOG_INFO);
        return va;
    }
};

// MmUnmapIoSpace(PVOID BaseAddress, SIZE_T NumberOfBytes)
class ApiMmUnmapIoSpace : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t va   = get_arg(cpu, 0);
        size_t   size = static_cast<size_t>(get_arg(cpu, 1));
        Debug::debug_msg("[MmUnmapIoSpace] va=" + hex64(va) + " size=" + std::to_string(size), LOG_INFO);
        return 0;
    }
};

// IoAllocateMdl(PVOID VirtualAddress, ULONG Length, BOOLEAN SecondaryBuffer, BOOLEAN ChargeQuota, PIRP Irp) -> PMDL
class ApiIoAllocateMdl : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t va      = get_arg(cpu, 0);
        uint32_t length  = static_cast<uint32_t>(get_arg(cpu, 1));
        uint8_t  sec_buf = static_cast<uint8_t>(get_arg(cpu, 2));
        uint8_t  quota   = static_cast<uint8_t>(get_arg(cpu, 3));
        uint64_t irp     = get_arg(cpu, 4);

        uint64_t mdl_addr = allocate(sizeof(EMU_MDL) + 0x40, 0, 0x206C644D, cpu); // 'Mdl '
        EMU_MDL mdl{};
        mdl.Size           = sizeof(EMU_MDL);
        mdl.MdlFlags       = 0;
        mdl.StartVa        = va & ~0xFFFULL;
        mdl.ByteOffset     = static_cast<uint32_t>(va & 0xFFFULL);
        mdl.ByteCount      = length;
        mdl.MappedSystemVa = va;

        cpu->mem_write(mdl_addr, &mdl, sizeof(EMU_MDL));

        Debug::debug_msg("[IoAllocateMdl] va=" + hex64(va) + " len=" + std::to_string(length) +
                         " -> mdl=" + hex64(mdl_addr), LOG_INFO);
        return mdl_addr;
    }
};

// IoFreeMdl(PMDL Mdl)
class ApiIoFreeMdl : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t mdl = get_arg(cpu, 0);
        Debug::debug_msg("[IoFreeMdl] mdl=" + hex64(mdl), LOG_INFO);
        return 0;
    }
};

// MmBuildMdlForNonPagedPool(PMDL Mdl)
class ApiMmBuildMdlForNonPagedPool : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t mdl_addr = get_arg(cpu, 0);
        EMU_MDL mdl{};
        if (cpu->mem_read(mdl_addr, &mdl, sizeof(EMU_MDL))) {
            mdl.MdlFlags |= 0x0004; // MDL_SOURCE_IS_NONPAGED_POOL
            mdl.MappedSystemVa = mdl.StartVa + mdl.ByteOffset;
            cpu->mem_write(mdl_addr, &mdl, sizeof(EMU_MDL));
        }
        Debug::debug_msg("[MmBuildMdlForNonPagedPool] mdl=" + hex64(mdl_addr), LOG_INFO);
        return 0;
    }
};

// MmProbeAndLockPages(PMDL Mdl, KPROCESSOR_MODE AccessMode, LOCK_OPERATION Operation)
class ApiMmProbeAndLockPages : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t mdl_addr = get_arg(cpu, 0);
        EMU_MDL mdl{};
        if (cpu->mem_read(mdl_addr, &mdl, sizeof(EMU_MDL))) {
            mdl.MdlFlags |= 0x0008; // MDL_PAGES_LOCKED
            cpu->mem_write(mdl_addr, &mdl, sizeof(EMU_MDL));
        }
        Debug::debug_msg("[MmProbeAndLockPages] mdl=" + hex64(mdl_addr), LOG_INFO);
        return 0;
    }
};

// MmUnlockPages(PMDL Mdl)
class ApiMmUnlockPages : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t mdl_addr = get_arg(cpu, 0);
        EMU_MDL mdl{};
        if (cpu->mem_read(mdl_addr, &mdl, sizeof(EMU_MDL))) {
            mdl.MdlFlags &= ~0x0008; // MDL_PAGES_LOCKED
            cpu->mem_write(mdl_addr, &mdl, sizeof(EMU_MDL));
        }
        Debug::debug_msg("[MmUnlockPages] mdl=" + hex64(mdl_addr), LOG_INFO);
        return 0;
    }
};

// MmMapLockedPagesSpecifyCache(PMDL Mdl, KPROCESSOR_MODE AccessMode, MEMORY_CACHING_TYPE CacheType, PVOID RequestedAddress, ULONG BugCheckOnFailure, ULONG Priority) -> PVOID
class ApiMmMapLockedPagesSpecifyCache : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t mdl_addr = get_arg(cpu, 0);
        EMU_MDL mdl{};
        uint64_t va = 0;
        if (cpu->mem_read(mdl_addr, &mdl, sizeof(EMU_MDL))) {
            va = mdl.MappedSystemVa ? mdl.MappedSystemVa : (mdl.StartVa + mdl.ByteOffset);
        }
        Debug::debug_msg("[MmMapLockedPagesSpecifyCache] mdl=" + hex64(mdl_addr) +
                         " -> va=" + hex64(va), LOG_INFO);
        return va;
    }
};

// MmMapLockedPages(PMDL Mdl, KPROCESSOR_MODE AccessMode) -> PVOID
class ApiMmMapLockedPages : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t mdl_addr = get_arg(cpu, 0);
        EMU_MDL mdl{};
        uint64_t va = 0;
        if (cpu->mem_read(mdl_addr, &mdl, sizeof(EMU_MDL))) {
            va = mdl.MappedSystemVa ? mdl.MappedSystemVa : (mdl.StartVa + mdl.ByteOffset);
        }
        Debug::debug_msg("[MmMapLockedPages] mdl=" + hex64(mdl_addr) + " -> va=" + hex64(va), LOG_INFO);
        return va;
    }
};

// MmUnmapLockedPages(PVOID BaseAddress, PMDL Mdl)
class ApiMmUnmapLockedPages : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t va  = get_arg(cpu, 0);
        uint64_t mdl = get_arg(cpu, 1);
        Debug::debug_msg("[MmUnmapLockedPages] va=" + hex64(va) + " mdl=" + hex64(mdl), LOG_INFO);
        return 0;
    }
};

// MmAllocatePagesForMdl(...) -> PMDL
class ApiMmAllocatePagesForMdl : public Api {
public:
    uint64_t call(CPU* cpu) override {
        size_t size = static_cast<size_t>(get_arg(cpu, 3));
        uint64_t va = allocate(size, 0, 0x206C644D, cpu);
        uint64_t mdl_addr = allocate(sizeof(EMU_MDL), 0, 0x206C644D, cpu);

        EMU_MDL mdl{};
        mdl.Size = sizeof(EMU_MDL);
        mdl.StartVa = va & ~0xFFFULL;
        mdl.ByteOffset = static_cast<uint32_t>(va & 0xFFFULL);
        mdl.ByteCount = static_cast<uint32_t>(size);
        mdl.MappedSystemVa = va;
        cpu->mem_write(mdl_addr, &mdl, sizeof(EMU_MDL));

        Debug::debug_msg("[MmAllocatePagesForMdl] size=" + std::to_string(size) + " -> mdl=" + hex64(mdl_addr), LOG_INFO);
        return mdl_addr;
    }
};

// MmAllocatePagesForMdlEx(...) -> PMDL
class ApiMmAllocatePagesForMdlEx : public Api {
public:
    uint64_t call(CPU* cpu) override {
        size_t size = static_cast<size_t>(get_arg(cpu, 3));
        uint64_t va = allocate(size, 0, 0x206C644D, cpu);
        uint64_t mdl_addr = allocate(sizeof(EMU_MDL), 0, 0x206C644D, cpu);

        EMU_MDL mdl{};
        mdl.Size = sizeof(EMU_MDL);
        mdl.StartVa = va & ~0xFFFULL;
        mdl.ByteOffset = static_cast<uint32_t>(va & 0xFFFULL);
        mdl.ByteCount = static_cast<uint32_t>(size);
        mdl.MappedSystemVa = va;
        cpu->mem_write(mdl_addr, &mdl, sizeof(EMU_MDL));

        Debug::debug_msg("[MmAllocatePagesForMdlEx] size=" + std::to_string(size) + " -> mdl=" + hex64(mdl_addr), LOG_INFO);
        return mdl_addr;
    }
};

// MmFreePagesFromMdl(PMDL Mdl)
class ApiMmFreePagesFromMdl : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t mdl = get_arg(cpu, 0);
        Debug::debug_msg("[MmFreePagesFromMdl] mdl=" + hex64(mdl), LOG_INFO);
        return 0;
    }
};

// MmAllocateContiguousMemory(SIZE_T NumberOfBytes, PHYSICAL_ADDRESS HighestAcceptableAddress) -> PVOID
class ApiMmAllocateContiguousMemory : public Api {
public:
    uint64_t call(CPU* cpu) override {
        size_t size = static_cast<size_t>(get_arg(cpu, 0));
        uint64_t va = allocate(size, 0, 0x6774434D, cpu); // 'MCtg'
        Debug::debug_msg("[MmAllocateContiguousMemory] size=" + std::to_string(size) +
                         " -> va=" + hex64(va), LOG_INFO);
        return va;
    }
};

// MmAllocateContiguousMemorySpecifyCache(...) -> PVOID
class ApiMmAllocateContiguousMemorySpecifyCache : public Api {
public:
    uint64_t call(CPU* cpu) override {
        size_t size = static_cast<size_t>(get_arg(cpu, 0));
        uint64_t va = allocate(size, 0, 0x6774434D, cpu);
        Debug::debug_msg("[MmAllocateContiguousMemorySpecifyCache] size=" + std::to_string(size) +
                         " -> va=" + hex64(va), LOG_INFO);
        return va;
    }
};

// MmFreeContiguousMemory(PVOID BaseAddress)
class ApiMmFreeContiguousMemory : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t va = get_arg(cpu, 0);
        Debug::debug_msg("[MmFreeContiguousMemory] va=" + hex64(va), LOG_INFO);
        return 0;
    }
};

// MmGetPhysicalAddress(PVOID BaseAddress) -> PHYSICAL_ADDRESS
class ApiMmGetPhysicalAddress : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t va = get_arg(cpu, 0);
        uint64_t pa = va & 0x00000000FFFFFFFFULL;
        Debug::debug_msg("[MmGetPhysicalAddress] va=" + hex64(va) + " -> pa=" + hex64(pa), LOG_INFO);
        return pa;
    }
};

// MmGetSystemRoutineAddress(PUNICODE_STRING SystemRoutineName) -> PVOID
class ApiMmGetSystemRoutineAddress : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t name_addr = get_arg(cpu, 0);
        std::string routine_name = read_unicode_string(cpu, name_addr);
        Debug::debug_msg("[MmGetSystemRoutineAddress] Resolving: \"" + routine_name + "\"", LOG_INFO);

        if (routine_name.empty()) return 0;

        // Check if trap entry exists
        for (size_t i = 0; i < cpu->trap_table.size(); ++i) {
            if (cpu->trap_table[i].function_name == routine_name) {
                uint64_t trap_addr = HOOK_TRAP_BASE + (i * 0x10);
                return trap_addr;
            }
        }

        // Dynamically add a trap entry so that invoking this routine resolves in APIDispatcher
        TrapEntry entry{};
        entry.function_name = routine_name;
        entry.module_name = "ntoskrnl.exe";
        cpu->trap_table.push_back(entry);

        uint64_t trap_addr = HOOK_TRAP_BASE + ((cpu->trap_table.size() - 1) * 0x10);
        return trap_addr;
    }
};

// MmSecureVirtualMemory(PVOID Address, SIZE_T Size, ULONG ProbeMode) -> HANDLE
class ApiMmSecureVirtualMemory : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t addr = get_arg(cpu, 0);
        size_t   size = static_cast<size_t>(get_arg(cpu, 1));
        uint64_t handle = 0xFFFFFA8000009999ULL;
        Debug::debug_msg("[MmSecureVirtualMemory] addr=" + hex64(addr) +
                         " size=" + std::to_string(size) + " -> handle=" + hex64(handle), LOG_INFO);
        return handle;
    }
};

// MmUnsecureVirtualMemory(HANDLE SecureHandle)
class ApiMmUnsecureVirtualMemory : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t handle = get_arg(cpu, 0);
        Debug::debug_msg("[MmUnsecureVirtualMemory] handle=" + hex64(handle), LOG_INFO);
        return 0;
    }
};
