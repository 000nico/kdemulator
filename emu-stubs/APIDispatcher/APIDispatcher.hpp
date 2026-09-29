#pragma once
#include "../emu-core/src/cpu/CPU.hpp"
#include "../emu-core/src/debug/Debug.hpp"
#include "../api/Api.hpp"
#include <unordered_map>

class APIDispatcher {
    public:
        APIDispatcher();
        bool resolve(CPU* cpu);
        void invoke(const std::string& name, CPU* cpu);

    private:
        std::unordered_map<std::string, Api*> apis;
        void register_api(const std::string& name, Api* api);
};
