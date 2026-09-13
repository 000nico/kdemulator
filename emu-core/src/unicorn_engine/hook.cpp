#include "UnicornEngine.hpp"
#include "hook.hpp"
#include "../debug/Debug.hpp"
#include <sstream>

static std::string hex64(uint64_t v) {
    std::ostringstream oss;
    oss << "0x" << std::uppercase << std::hex << v;
    return oss.str();
}

void uc_code_hook_trampoline(uc_engine* uc, uint64_t address, uint32_t size, void* user_data) {
    HookContext* ctx = static_cast<HookContext*>(user_data); 
    ctx->user_callback(ctx->cpu, address, ctx->user_data);
}

bool UnicornEngine::add_code_hook(uint64_t begin, uint64_t end, CodeHookFn callback, void* user_data) {
    auto* ctx = new HookContext{ this, callback, user_data }; 

    hooks.push_back(ctx);

    uc_hook hh;
    uc_err err = uc_hook_add(this->uc, &hh, UC_HOOK_CODE, (void*)uc_code_hook_trampoline, ctx, begin, end);

    if (err != UC_ERR_OK) {
        Debug::debug_msg(
            "hook: uc_hook_add failed [begin=" + hex64(begin) + " end=" + hex64(end) + "] err=" + uc_strerror(err) + "\n",
            LOG_ERROR
        );
        return false;
    }

    Debug::debug_msg(
        "hook: code hook registered [begin=" + hex64(begin) + " end=" + hex64(end) + "]\n",
        LOG_INFO
    );
    return true;
}
