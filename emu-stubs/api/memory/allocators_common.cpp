#include <cstdint>
#include "allocators_common.hpp"
#include "../emu-core/src/memory/layout.hpp"
#include "../emu-core/src/cpu/perms.hpp"
#include "../emu-core/src/debug/Debug.hpp"

std::vector<AllocationInfo> activeAllocations;
uint64_t nonPagedCurrentAddress = NON_PAGED_POOL_BASE;
uint64_t pagedCurrentAddress = PAGED_POOL_BASE;

bool isPagedPool(uint32_t poolType) {
    switch (poolType) {
        case 1:  // PagedPool
        case 5:  // PagedPoolCacheAligned
        case 33: // PagedPoolSession
        case 37: // PagedPoolCacheAlignedSession
            return true; 
            
        default:
            return false; // Non-Paged
    }
}

uint64_t allocate(size_t size, uint32_t poolType, uint32_t tag, CPU* cpu){
    const size_t PAGE_SIZE = 0x1000;
    size_t aligned_size = (size + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);
    
    bool paged = isPagedPool(poolType);
    
    AllocationInfo ai = {};
    ai.size = size;
    ai.tag = tag;
    
    if(!paged){
        ai.address = nonPagedCurrentAddress;
        cpu->mem_map(nonPagedCurrentAddress, aligned_size, PROT_READ | PROT_WRITE);
        nonPagedCurrentAddress += aligned_size;
        ai.isPaged = false;
    }
    
    else if (paged){
        ai.address = pagedCurrentAddress;
        cpu->mem_map(ai.address, aligned_size, PROT_READ | PROT_WRITE);
        pagedCurrentAddress += aligned_size;
        ai.isPaged = true;
    }

    Debug::debug_msg("Allocated memory at address " + std::to_string(ai.address) + ", size = " + std::to_string(size), LOG_INFO);

    activeAllocations.push_back(ai);
    return ai.address;
}

// it doesnt actually "free"
void free_mem(uint64_t address){
    bool found = false;
    int i = 0;
    
    while(found == false && i < activeAllocations.size()){
        if(activeAllocations[i].address == address){
            found = true;
            break;
        }
        else 
            i++;
    }

    if(found) {
        activeAllocations.erase(activeAllocations.begin() + i);
    }
}