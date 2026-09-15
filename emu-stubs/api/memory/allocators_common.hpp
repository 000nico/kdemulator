#pragma once
#include "../emu-core/src/cpu/CPU.hpp"
#include <cstdint>
#include<vector>

#define POOL_IS_PAGED     1
#define POOL_IS_NONPAGED  2

struct AllocationInfo {
    uint64_t address;
    size_t size;
    uint32_t tag;
    bool isPaged;
};

extern std::vector<AllocationInfo> activeAllocations;
extern uint64_t nonPagedCurrentAddress;
extern uint64_t pagedCurrentAddress;


uint64_t allocate(size_t size, uint32_t poolType, uint32_t tag, CPU* cpu);
void free_mem(uint64_t address);
bool isPagedPool(uint32_t poolType);