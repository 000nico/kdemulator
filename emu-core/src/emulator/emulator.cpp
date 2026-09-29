#include "Emulator.hpp"
#include "../cpu/perms.hpp"
#include "../memory/layout.hpp"
#include "../kernel/allocator.hpp"
#include "ioctl_injection.hpp"
#include "hooks.hpp"
#include <cstdint>
#include <hex.hpp>

Emulator::Emulator(CPU* cpu) : cpu(cpu) {
    stack.base = STACK_BASE;
    stack.size = STACK_SIZE;
    heap.base = HEAP_BASE;
    heap.size = HEAP_SIZE;
    disasm.init();
}

bool Emulator::load_driver(PE* pe){
    if(!pe) {
        Debug::debug_msg("load_driver: PE pointer is null\n", LOG_ERROR);
        return false;
    }
    if(!this->pe_manager.load_pe(this->cpu, *pe, CODE_BASE, &this->cpu->trap_table)) return false;

    return true;
}

static IMAGE_LOAD_CONFIG_DIRECTORY64* get_load_config_directory(PE* pe) {
    uint32_t va = pe->image_optional_header.LoadConfigurationDirectoryVA;
    if (!va) return nullptr;

    for (auto& sec : pe->sections) {
        if (va < sec.VirtualAddress || va >= sec.VirtualAddress + sec.PhysicalAddress_VirtualSize)
            continue;

        uint32_t off = sec.PointerToRawData + (va - sec.VirtualAddress);
        return reinterpret_cast<IMAGE_LOAD_CONFIG_DIRECTORY64*>(pe->raw_data.data() + off);
    }

    return nullptr;
}

void security_cookie_patch(CPU* cpu, PE* pe) {
    auto* load_cfg = get_load_config_directory(pe);
    if (!load_cfg) {
        Debug::debug_msg("security_cookie_patch: no LOAD_CONFIG_DIRECTORY found, skipping", LOG_WARN);
        return;
    }

    uint64_t cookie_va = load_cfg->SecurityCookie;
    if (!cookie_va) {
        Debug::debug_msg("security_cookie_patch: SecurityCookie is 0, skipping", LOG_WARN);
        return;
    }

    uint64_t rva = cookie_va - pe->image_optional_header.ImageBase;
    uint64_t cookie_addr = CODE_BASE + rva;

    uint64_t fixed_cookie = 0xDEADC0DECAFEBABEULL;

    if (!cpu->mem_write(cookie_addr, &fixed_cookie, sizeof(fixed_cookie))) {
        Debug::debug_msg("security_cookie_patch: mem_write failed at " + hex64(cookie_addr), LOG_ERROR);
        return;
    }

    Debug::debug_msg("security_cookie_patch: patched cookie at " + hex64(cookie_addr), LOG_INFO);
}

void mam_paged_nonpaged_pools(CPU* cpu) {
    cpu->mem_map(NON_PAGED_POOL_BASE, NON_PAGED_POOL_SIZE, PROT_READ | PROT_WRITE);
    cpu->mem_map(PAGED_POOL_BASE, PAGED_POOL_SIZE, PROT_READ | PROT_WRITE);\
    Debug::debug_msg("Paged and non paged pool mapped", LOG_INFO);
}

void map_stack_and_heap(CPU* cpu){
    cpu->mem_map(STACK_BASE, STACK_SIZE, PROT_READ | PROT_WRITE);
    cpu->mem_map(HEAP_BASE, HEAP_SIZE, PROT_READ | PROT_WRITE);
    Debug::debug_msg("Stack and heap mapped", LOG_INFO);
}

void set_rbp_rsp(CPU* cpu){
    cpu->set_register(REG_RSP, STACK_BASE + STACK_SIZE - 0x1000);
    cpu->set_register(REG_RBP, STACK_BASE + STACK_SIZE - 0x1000);
    Debug::debug_msg("RBP and RSP set", LOG_INFO);
}

void set_entry_point(CPU* cpu, PE* pe){
    uint64_t entry_point = CODE_BASE + pe->image_optional_header.AddressOfEntryPoint;
    cpu->set_register(REG_RIP, entry_point);
}

bool Emulator::start(PE* pe){
    if(!pe) {
        Debug::debug_msg("start: PE pointer is null, no driver loaded\n", LOG_ERROR);
        return false;
    }

    mam_paged_nonpaged_pools(this->cpu);
    map_stack_and_heap(this->cpu);
    set_rbp_rsp(this->cpu);
    set_entry_point(this->cpu, pe);
    allocate_kuser(this->cpu);
    allocate_driver_object(this->cpu, pe);
    security_cookie_patch(this->cpu, pe);

    addCodeHookCallback(this->cpu, pe, &this->disasm);
    addTrapHookCallback(this->cpu, &this->dispatcher);

    uint64_t ret_addr = get_driver_entry_ret_absolute_address(pe, this->cpu, this->disasm);
    add_hook_on_driver_entry_ret(this->cpu, ret_addr);

    // pass arguments
    cpu->set_register(REG_RCX, STRUCT_BASE + OFF_DEVOBJ );
    return cpu->start(CODE_BASE + pe->image_optional_header.AddressOfEntryPoint);
}
