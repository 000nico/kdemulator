#include "panels.hpp"
#include <sstream>
#include <iomanip>

using namespace ftxui;

std::string hex64(uint64_t value) {
    std::ostringstream oss;
    oss << "0x" << std::setw(16) << std::setfill('0') << std::hex << value;
    return oss.str();
}

Element build_layout(const RegisterSnapshot& regs, const std::vector<std::string>& logs, const std::vector<std::string>& command_history, Element command_input_render) {
    auto registers_panel = window(text("Registers"), vbox({
        text("RAX: " + hex64(regs.rax)),
        text("RBX: " + hex64(regs.rbx)),
        text("RIP: " + hex64(regs.rip)),
        text("RSP: " + hex64(regs.rsp)),
    }));

    auto info_panel = window(text("Driver Info"), vbox({
        text("Name: example.sys"),
        text("Base: 0x0000000140000000"),
        text("Entry: 0x0000000140001000"),
    }));

    auto top_row = hbox({
        registers_panel | flex,
        info_panel | flex,
    });

    // build logs panel content from real log lines
    std::vector<Element> log_lines;
    for (const auto& line : logs) {
        log_lines.push_back(text(line));
    }
    for (const auto& cmd : command_history) {
        log_lines.push_back(text(cmd) | color(Color::GrayLight));
    }
    auto logs_panel = window(text("Logs"), vbox(log_lines));

    auto command_panel = window(text("Command"), command_input_render);

    return vbox({
        top_row,
        logs_panel | flex,
        command_panel,
    });
}