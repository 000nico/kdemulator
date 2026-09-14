#include "panels.hpp"
#include <hex.hpp>
#include <iomanip>

using namespace ftxui;

Element build_layout(
    const RegisterSnapshot& regs,
    const std::vector<std::string>& logs,
    const std::vector<TraceEntry>& traces,
    const std::vector<std::string>& command_history,
    Element command_input_render,
    const std::string& current_input,
    const std::string& driver_name,
    int scroll_offset
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
        // Line format: "(HH:MM:SS) [LEVEL] message"
        size_t close_paren = line.find(") [");
        if (line.front() == '(' && close_paren != std::string::npos) {
            std::string time_part = line.substr(0, close_paren + 1); // "(HH:MM:SS)"
            size_t tag_end = line.find(']', close_paren + 2);
            if (tag_end != std::string::npos) {
                std::string tag_part = line.substr(close_paren + 2, tag_end - (close_paren + 2) + 1); // "[INFO]"
                std::string msg_part = (tag_end + 1 < line.size()) ? line.substr(tag_end + 1) : "";
                if (!msg_part.empty() && msg_part.front() == ' ') {
                    msg_part = msg_part.substr(1);
                }

                Color tag_color = Color::Cyan;
                if (tag_part == "[ERROR]") tag_color = Color::Red;
                else if (tag_part == "[WARN]") tag_color = Color::Yellow;
                else if (tag_part == "[INFO]") tag_color = Color::Cyan;

                log_lines.push_back(hbox({
                    text(time_part + " ") | color(Color::GrayLight),
                    text(tag_part + " ") | bold | color(tag_color),
                    text(msg_part) | color(Color::White),
                }));
                continue;
            }
        }

        // Fallback formatting if not matching standard format
        if (line.find("[ERROR]") != std::string::npos) {
            size_t pos = line.find("[ERROR]");
            log_lines.push_back(hbox({
                text(line.substr(0, pos)) | color(Color::GrayLight),
                text("[ERROR] ") | bold | color(Color::Red),
                text(pos + 7 < line.size() ? line.substr(pos + 7) : "") | color(Color::White),
            }));
        } else if (line.find("[WARN]") != std::string::npos) {
            size_t pos = line.find("[WARN]");
            log_lines.push_back(hbox({
                text(line.substr(0, pos)) | color(Color::GrayLight),
                text("[WARN] ") | bold | color(Color::Yellow),
                text(pos + 6 < line.size() ? line.substr(pos + 6) : "") | color(Color::White),
            }));
        } else if (line.find("[INFO]") != std::string::npos) {
            size_t pos = line.find("[INFO]");
            log_lines.push_back(hbox({
                text(line.substr(0, pos)) | color(Color::GrayLight),
                text("[INFO] ") | bold | color(Color::Cyan),
                text(pos + 6 < line.size() ? line.substr(pos + 6) : "") | color(Color::White),
            }));
        } else {
            log_lines.push_back(text(line) | color(Color::White));
        }
    }
    if (!log_lines.empty()) {
        int total = (int)log_lines.size();
        int focus_idx = total - 1 - scroll_offset;
        if (focus_idx < 0) focus_idx = 0;
        if (focus_idx >= total) focus_idx = total - 1;
        log_lines[focus_idx] = log_lines[focus_idx] | focus;
    }

    std::string title = "Logs (Scroll: PgUp/PgDn/Wheel)";
    if (scroll_offset > 0) {
        title += " [Scrolled +" + std::to_string(scroll_offset) + "]";
    }
    auto logs_panel = window(text(title), vbox(std::move(log_lines)) | vscroll_indicator | yframe | flex);

    // build command history panel (separate distinct panel)
    std::vector<Element> cmd_history_lines;
    if (command_history.empty()) {
        cmd_history_lines.push_back(text("  [No commands executed yet]") | color(Color::GrayDark));
    } else {
        for (const auto& cmd : command_history) {
            std::string display_cmd = cmd;
            if (display_cmd.rfind("> ", 0) == 0) {
                display_cmd = display_cmd.substr(2);
            }
            cmd_history_lines.push_back(
                hbox({
                    text("❯ ") | bold | color(Color::MagentaLight),
                    text(display_cmd) | bold | color(Color::White),
                })
            );
        }
    }
    auto cmd_history_panel = window(text("Command History"), vbox(std::move(cmd_history_lines)) | vscroll_indicator | yframe);

    // build execution trace / disassembly panel (on the right - always visible)
    std::vector<Element> trace_lines;
    if (traces.empty()) {
        trace_lines.push_back(text("  [No instructions executed yet]") | color(Color::GrayDark));
    } else {
        size_t total_traces = traces.size();
        for (size_t i = 0; i < total_traces; ++i) {
            bool is_current = (i == total_traces - 1);
            std::string prefix = is_current ? "-> " : "   ";
            std::string addr_str = hex64(traces[i].address);
            std::string disasm_text = traces[i].instruction;

            if (is_current) {
                trace_lines.push_back(
                    hbox({
                        text(prefix) | bold | color(Color::Green),
                        text(addr_str + ": ") | bold | color(Color::Yellow),
                        text(disasm_text) | bold | color(Color::White),
                    }) | focus
                );
            } else {
                trace_lines.push_back(
                    hbox({
                        text(prefix) | color(Color::GrayDark),
                        text(addr_str + ": ") | color(Color::GrayLight),
                        text(disasm_text) | color(Color::White),
                    })
                );
            }
        }
    }
    auto trace_panel = window(text("Disassembly Trace (Live)"), vbox(std::move(trace_lines)) | vscroll_indicator | yframe | flex);

    auto left_column = vbox({
        logs_panel | flex,
        cmd_history_panel | size(HEIGHT, EQUAL, 6),
    });

    auto middle_row = hbox({
        left_column | flex,
        separator(),
        trace_panel | flex,
    });

    // Interactive command suggestions when typing '/'
    std::vector<std::string> all_commands = {
        "/load <path>  - Load .sys driver file into memory",
        "/start        - Start driver execution from EntryPoint",
        "/step         - Execute a single CPU instruction",
        "/regs         - Refresh registers on screen",
        "/trace        - Toggle trace mode (on/off)",
        "/clean        - Clear logs panel",
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
        middle_row | flex,
        command_panel,
    });
}