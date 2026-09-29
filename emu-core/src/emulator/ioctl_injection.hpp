#pragma once
#include "../pe/format.hpp"
#include "../cpu/CPU.hpp"
#include "../disasm/Disasm.hpp"

static const char* IrpMjNames[] = {
    "CREATE", "CREATE_NAMED_PIPE", "CLOSE", "READ", "WRITE",
    "QUERY_INFORMATION", "SET_INFORMATION", "QUERY_EA", "SET_EA",
    "FLUSH_BUFFERS", "QUERY_VOLUME_INFORMATION", "SET_VOLUME_INFORMATION",
    "DIRECTORY_CONTROL", "FILE_SYSTEM_CONTROL", "DEVICE_CONTROL",
    "INTERNAL_DEVICE_CONTROL", "SHUTDOWN", "LOCK_CONTROL", "CLEANUP",
    "CREATE_MAILSLOT", "QUERY_SECURITY", "SET_SECURITY", "POWER",
    "SYSTEM_CONTROL", "DEVICE_CHANGE", "QUERY_QUOTA", "SET_QUOTA", "PNP"
};

uint64_t get_driver_entry_ret_absolute_address(PE* pe, CPU* cpu, Disasm& disasm);
void add_hook_on_driver_entry_ret(CPU* cpu, uint64_t address);