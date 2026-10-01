#include "../emu-core/src/emulator/Emulator.hpp"
#include "../emu-core/src/unicorn_engine/UnicornEngine.hpp"
#include "../emu-core/src/emulator/ioctl_injection.hpp"
#include "src/out/Output.hpp"
#include "src/in/Input.hpp"
#include <sstream>
#include <filesystem>

int main() {
    InputHandler input;
    UnicornEngine cpu;
    Output output(input, &cpu);
    Emulator emulator(&cpu);

    PE current_pe;
    bool pe_loaded = false;

    // CLI command dispatcher
    input.set_on_command([&](const std::string& cmd_line) {
        output.reset_scroll();
        std::istringstream iss(cmd_line);
        std::string cmd;
        iss >> cmd;

        if (cmd == "/" || cmd == "/help") {
            Debug::debug_msg("=== Available Commands ===\n", LOG_INFO);
            Debug::debug_msg("  /load <path>  - Load .sys driver file into memory\n", LOG_INFO);
            Debug::debug_msg("  /start        - Start driver execution from EntryPoint\n", LOG_INFO);
            Debug::debug_msg("  /ioctl <code> [hex_bytes...] - Inject an IOCTL into IRP_MJ_DEVICE_CONTROL\n", LOG_INFO);
            Debug::debug_msg("                  <code>       : IOCTL code (hex 0x... or decimal)\n", LOG_INFO);
            Debug::debug_msg("                  [hex_bytes]  : optional input buffer bytes (e.g. DEADBEEF)\n", LOG_INFO);
            Debug::debug_msg("  /step         - Execute a single CPU instruction\n", LOG_INFO);
            Debug::debug_msg("  /regs         - Refresh registers on screen\n", LOG_INFO);
            Debug::debug_msg("  /trace        - Toggle trace mode (on/off)\n", LOG_INFO);
            Debug::debug_msg("  /clean        - Clear the logs panel\n", LOG_INFO);
            Debug::debug_msg("  /help         - Show this help menu\n", LOG_INFO);
            Debug::debug_msg("  /exit         - Exit the emulator\n", LOG_INFO);
        }
        else if (cmd == "/clean" || cmd == "/cls" || cmd == "/clear") {
            output.clear_logs();
        }
        else if (cmd == "/trace") {
            Debug::traceMode = !Debug::traceMode;
            Debug::debug_msg("Trace mode " + std::string(Debug::traceMode ? "ENABLED" : "DISABLED") + ".\n", LOG_INFO);
            output.refresh();
        }

        else if (cmd == "/start") {
            // Error handling: Check if a driver PE is already loaded
            if (!pe_loaded) {
                Debug::debug_msg("Cannot start: No driver loaded. Use '/load <path.sys>' first.\n", LOG_ERROR);
                return;
            }

            Debug::debug_msg("Starting driver emulation from EntryPoint...\n", LOG_INFO);
            
            // Execute and verify return status
            bool ok = emulator.start(&current_pe);
            if (!ok) {
                Debug::debug_msg("emulator.start() failed during execution.\n", LOG_ERROR);
            } else {
                Debug::debug_msg("Driver execution finished successfully.\n", LOG_INFO);
            }

            // Real-time refresh of UI
            output.refresh();
        }
        else if (cmd == "/load") {
            std::string path;
            iss >> path;
            if (path.empty()) {
                Debug::debug_msg("Missing argument. Usage: /load <path_to_driver.sys>\n", LOG_ERROR);
                return;
            }

            PEManager pe_mgr;
            if (!pe_mgr.read_pe_from_disk(const_cast<char*>(path.c_str()), &current_pe)) {
                Debug::debug_msg("Failed to read or parse PE file: " + path + "\n", LOG_ERROR);
                return;
            }

            if (!emulator.load_driver(&current_pe)) {
                Debug::debug_msg("Failed to map driver sections into memory.\n", LOG_ERROR);
                return;
            }

            pe_loaded = true;
            Debug::clear_trace_history();
            output.clear_traces();
            output.set_driver_name(std::filesystem::path(path).filename().string());
            Debug::debug_msg("Driver loaded and mapped into memory successfully.\n", LOG_INFO);
            
            output.refresh();
        }
        else if (cmd == "/step") {
            if (!pe_loaded) {
                Debug::debug_msg("No driver is currently loaded.\n", LOG_ERROR);
                return;
            }
            CpuStatus st = cpu.step_cpu();
            if (st == CpuStatus::HOOK_TRAP) {
                Debug::debug_msg("CPU intercepted Hook Trap.\n", LOG_WARN);
            } else {
                Debug::debug_msg("Instruction step executed.\n", LOG_INFO);
            }
            output.refresh();
        }
        else if (cmd == "/regs") {
            output.refresh();
            Debug::debug_msg("Registers refreshed.\n", LOG_INFO);
        }
        else if (cmd == "/ioctl") {
            if (!pe_loaded) {
                Debug::debug_msg("Cannot inject IOCTL: no driver loaded. Use '/load <path.sys>' and '/start' first.\n", LOG_ERROR);
                return;
            }

            // --- Parse IOCTL code (mandatory) ---
            std::string code_str;
            iss >> code_str;
            if (code_str.empty()) {
                Debug::debug_msg("Usage: /ioctl <code> [hex_bytes...]\n", LOG_ERROR);
                Debug::debug_msg("  <code>      : IOCTL code in hex (0x...) or decimal\n", LOG_ERROR);
                Debug::debug_msg("  [hex_bytes] : optional input buffer as a hex string (e.g. DEADBEEF 0102)\n", LOG_ERROR);
                return;
            }

            uint32_t ioctl_code = 0;
            try {
                ioctl_code = static_cast<uint32_t>(std::stoull(code_str, nullptr, 0));
            } catch (...) {
                Debug::debug_msg("Invalid IOCTL code: '" + code_str + "'. Use hex (0x...) or decimal.\n", LOG_ERROR);
                return;
            }

            // --- Parse optional input buffer (hex tokens) ---
            std::vector<uint8_t> input_buf;
            std::string hex_token;
            while (iss >> hex_token) {
                // Accept tokens with or without 0x prefix; strip it if present
                std::string raw = hex_token;
                if (raw.size() >= 2 && raw[0] == '0' && (raw[1] == 'x' || raw[1] == 'X'))
                    raw = raw.substr(2);

                if (raw.empty() || raw.size() % 2 != 0) {
                    Debug::debug_msg("Bad hex token '" + hex_token + "': must be an even number of hex digits (e.g. DEADBEEF).\n", LOG_ERROR);
                    return;
                }
                for (size_t i = 0; i < raw.size(); i += 2) {
                    try {
                        uint8_t byte = static_cast<uint8_t>(std::stoul(raw.substr(i, 2), nullptr, 16));
                        input_buf.push_back(byte);
                    } catch (...) {
                        Debug::debug_msg("Invalid hex byte in token '" + hex_token + "'.\n", LOG_ERROR);
                        return;
                    }
                }
            }

            constexpr size_t DEFAULT_OUTPUT_LEN = 0x1000;

            Debug::debug_msg(
                "Injecting IOCTL 0x" + [&]{
                    std::ostringstream ss; ss << std::hex << std::uppercase << ioctl_code; return ss.str();
                }() +
                " | input_len=" + std::to_string(input_buf.size()) +
                " | output_len=" + std::to_string(DEFAULT_OUTPUT_LEN) + "\n",
                LOG_INFO
            );

            invoke_major_function(
                &cpu,
                ioctl_code,
                input_buf.empty() ? nullptr : input_buf.data(),
                input_buf.size(),
                DEFAULT_OUTPUT_LEN
            );

            output.refresh();
        }
        else if (cmd == "/exit" || cmd == "/quit") {
            output.screen.ExitLoopClosure()();
        }
        else {
            Debug::debug_msg("Unknown command: " + cmd + ". Type '/' for available commands.\n", LOG_WARN);
        }

    });

    output.init();
    output.screen.Loop(output.renderer);

    return 0;
}

