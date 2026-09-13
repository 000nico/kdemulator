#pragma once
#include <vector>
#include <string>
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include "panels.hpp"
#include "../in/Input.hpp"
#include "../../../emu-core/src/cpu/CPU.hpp"

class Output {
    public:
        Output(InputHandler& input, CPU* cpu = nullptr) :
            cpu(cpu),
            screen(ftxui::ScreenInteractive::Fullscreen()),
            renderer(ftxui::Renderer(input.input_with_submit, [&input, this] {
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
                    input.get_command_history(),
                    input.input_with_submit->Render(),
                    input.get_current_input(),
                    driver_name
                );
            }))
        {}
        ~Output();

        void init();

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
        std::string driver_name = "None";
};
