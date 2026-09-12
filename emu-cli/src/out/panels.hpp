#pragma once
#include <ftxui/dom/elements.hpp>
#include <vector>
#include <string>

struct RegisterSnapshot {
    uint64_t rax, rbx, rip, rsp;
};

ftxui::Element build_layout(
    const RegisterSnapshot& regs,
    const std::vector<std::string>& logs,
    const std::vector<std::string>& command_history,
    ftxui::Element command_input_render
);