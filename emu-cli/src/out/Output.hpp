#pragma once
#include <vector>
#include <string>
#include <algorithm>
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/component/mouse.hpp>
#include "panels.hpp"
#include "../in/Input.hpp"
#include "../../../emu-core/src/cpu/CPU.hpp"

class Output {
    public:
        Output(InputHandler& input, CPU* cpu = nullptr) :
            cpu(cpu),
            screen(ftxui::ScreenInteractive::Fullscreen())
        {
            auto event_handler = ftxui::CatchEvent(input.input_with_submit, [this, &input](ftxui::Event event) {
                int total_lines = (int)logs.size();
                int max_scroll = total_lines > 1 ? total_lines - 1 : 0;


                if (event == ftxui::Event::PageUp) {
                    scroll_offset = std::min(scroll_offset + 10, max_scroll);
                    return true;
                }
                if (event == ftxui::Event::PageDown) {
                    scroll_offset = std::max(0, scroll_offset - 10);
                    return true;
                }
                if (event == ftxui::Event::Home) {
                    scroll_offset = max_scroll;
                    return true;
                }
                if (event == ftxui::Event::End) {
                    scroll_offset = 0;
                    return true;
                }
                if (event.is_mouse()) {
                    if (event.mouse().button == ftxui::Mouse::WheelUp) {
                        scroll_offset = std::min(scroll_offset + 3, max_scroll);
                        return true;
                    }
                    if (event.mouse().button == ftxui::Mouse::WheelDown) {
                        scroll_offset = std::max(0, scroll_offset - 3);
                        return true;
                    }
                }
                return false;
            });

            renderer = ftxui::Renderer(event_handler, [&input, this] {
                RegisterSnapshot current_regs{};
                if (this->cpu) {
                    current_regs.rax = this->cpu->get_register(REG_RAX);
                    current_regs.rbx = this->cpu->get_register(REG_RBX);
                    current_regs.rcx = this->cpu->get_register(REG_RCX);
                    current_regs.rdx = this->cpu->get_register(REG_RDX);
                    current_regs.rsi = this->cpu->get_register(REG_RSI);
                    current_regs.rdi = this->cpu->get_register(REG_RDI);
                    current_regs.rip = this->cpu->get_register(REG_RIP);
                    current_regs.rsp = this->cpu->get_register(REG_RSP);
                }
                return build_layout(
                    current_regs,
                    logs,
                    traces,
                    input.get_command_history(),
                    input.input_with_submit->Render(),
                    input.get_current_input(),
                    driver_name,
                    scroll_offset
                );
            });
        }
        ~Output();

        void init();

        void reset_scroll() {
            scroll_offset = 0;
            refresh();
        }

        void clear_logs() {
            logs.clear();
            scroll_offset = 0;
            refresh();
        }

        void clear_traces() {
            traces.clear();
            refresh();
        }


        void set_driver_name(const std::string& name) {
            driver_name = name;
            refresh();
        }

        void set_cpu(CPU* new_cpu) {
            cpu = new_cpu;
            refresh();
        }

        void refresh() {
            screen.PostEvent(ftxui::Event::Custom);
        }

        ftxui::ScreenInteractive screen;
        ftxui::Component renderer;

    private:
        CPU* cpu = nullptr;
        std::vector<std::string> logs;
        std::vector<TraceEntry> traces;
        std::string driver_name = "None";
        int scroll_offset = 0;
};

