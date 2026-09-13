#include "panels.hpp"
#include <sstream>
#include <iomanip>

using namespace ftxui;

std::string hex64(uint64_t value) {
    std::ostringstream oss;
    oss << "0x" << std::setw(16) << std::setfill('0') << std::hex << value;
    return oss.str();
}

Element build_layout(
    const RegisterSnapshot& regs,
    const std::vector<std::string>& logs,
    const std::vector<std::string>& command_history,
    Element command_input_render,
    const std::string& current_input,
    const std::string& driver_name
) {
    auto registers_panel = window(text("Registers (Live)"), hbox({
        vbox({
            text("RAX: " + hex64(regs.rax)) | color(Color::Cyan),
            text("RBX: " + hex64(regs.rbx)) | color(Color::Cyan),
            text("RCX: " + hex64(regs.rcx)) | color(Color::Cyan),
            text("RDX: " + hex64(regs.rdx)) | color(Color::Cyan),
        }) | flex,
        separator(),
        vbox({
            text("RSI: " + hex64(regs.rsi)) | color(Color::Yellow),
            text("RDI: " + hex64(regs.rdi)) | color(Color::Yellow),
            text("RIP: " + hex64(regs.rip)) | bold | color(Color::Green),
            text("RSP: " + hex64(regs.rsp)) | color(Color::Magenta),
        }) | flex,
    }));

    auto info_panel = window(text("Driver Info"), vbox({
        text("Driver: " + (driver_name.empty() ? "None" : driver_name)) | color(Color::Cyan),
        text("Base:   0x0000000140000000"),
        text("Stack:  0x00000000FFFF0000"),
        text("Heap:   0x0000000010000000"),
    }));

    auto top_row = hbox({
        registers_panel | flex_grow,
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
    if (!log_lines.empty()) {
        log_lines.back() = log_lines.back() | focus;
    }
    auto logs_panel = window(text("Logs"), vbox(std::move(log_lines)) | vscroll_indicator | yframe | flex);

    // Interactive command suggestions when typing '/'
    std::vector<std::string> all_commands = {
        "/load <path>  - Load .sys driver file into memory",
        "/start        - Start driver execution from EntryPoint",
        "/step         - Execute a single CPU instruction",
        "/regs         - Refresh registers on screen",
        "/help         - Show command help menu",
        "/exit         - Exit the emulator"
    };

    std::vector<Element> suggestion_elements;
    if (!current_input.empty() && current_input[0] == '/') {
        std::string prefix = current_input.substr(0, current_input.find(' '));
        for (const auto& item : all_commands) {
            if (prefix == "/" || item.rfind(prefix, 0) == 0) {
                suggestion_elements.push_back(text("  • " + item) | color(Color::Cyan));
            }
        }
    }

    Element cmd_content = command_input_render;
    if (!suggestion_elements.empty()) {
        cmd_content = vbox({
            command_input_render,
            separator(),
            text("Available commands:") | bold | color(Color::Yellow),
            vbox(std::move(suggestion_elements))
        });
    }

    auto command_panel = window(text("Command (type '/' for commands)"), cmd_content);

    return vbox({
        top_row,
        logs_panel | flex,
        command_panel,
    });
}