#include "UnicornEngine.hpp"
#include "hook.hpp"

void uc_code_hook_trampoline(uc_engine* uc, uint64_t address, uint32_t size, void* user_data) {
    HookContext* ctx = static_cast<HookContext*>(user_data); 
    ctx->user_callback(ctx->cpu, address, ctx->user_data);
}

bool UnicornEngine::add_code_hook(uint64_t begin, uint64_t end, CodeHookFn callback, void* user_data) {
    auto* ctx = new HookContext{ this, callback, user_data }; 

    hooks.push_back(ctx);

    uc_hook hh;
    uc_err err = uc_hook_add(this->uc, &hh, UC_HOOK_CODE, (void*)uc_code_hook_trampoline, ctx, begin, end);
    return err == UC_ERR_OK;
}