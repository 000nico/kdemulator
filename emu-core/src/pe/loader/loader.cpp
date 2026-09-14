#include "loader.hpp"
#include "../../debug/Debug.hpp"

bool PEManager::load_pe(CPU* cpu, PE pe, uintptr_t address, std::vector<TrapEntry>* trap_table){
    if(!load_headers(cpu, pe, address))  return false;
    Debug::debug_msg("pe loader: headers loaded\n", LOG_INFO);
    if(!load_sections(cpu, pe, address)) return false;
    Debug::debug_msg("pe loader: sections loaded\n", LOG_INFO);
    if(!resolve_reloc(cpu, pe, address)) return false;
    Debug::debug_msg("pe loader: .relocs solved\n", LOG_INFO);
    if(!resolveIat(cpu, pe, address, trap_table))    return false;
    Debug::debug_msg("pe loader: iat resolved\n", LOG_INFO);
    if(!applyMemProtect(cpu,pe, address))return false;
    Debug::debug_msg("pe loader: mem protections applied\n", LOG_INFO);
    
    return true;
}
