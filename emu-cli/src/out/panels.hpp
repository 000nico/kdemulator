#pragma once
#include <ftxui/dom/elements.hpp>
#include <vector>
#include <string>
#include <cstdint>
#include "../../../emu-core/src/debug/Debug.hpp"

struct RegisterSnapshot {
    uint64_t rax, rbx, rcx, rdx;
    uint64_t rsi, rdi, rip, rsp;
};

ftxui::Element build_layout(
    const RegisterSnapshot& regs,
    const std::vector<std::string>& logs,
    const std::vector<TraceEntry>& traces,
    const std::vector<std::string>& command_history,
    ftxui::Element command_input_render,
    const std::string& current_input = "",
    const std::string& driver_name = "None",
    int scroll_offset = 0
);

