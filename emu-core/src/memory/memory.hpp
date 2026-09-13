#pragma once
#include <cstdint>

struct Stack {
    uintptr_t base;
    size_t size;
};

struct Heap {
    uintptr_t base;
    size_t size;
};