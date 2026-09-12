#pragma once
#include "../cpu/CPU.hpp"

struct HookContext {
    CPU* cpu;
    CodeHookFn user_callback;
    void* user_data;
};