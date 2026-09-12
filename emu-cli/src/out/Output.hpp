#pragma once
#include <vector>
#include <string>
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include "panels.hpp"
#include "../in/Input.hpp"

class Output {
    public:
        Output(InputHandler& input) :
            screen(ftxui::ScreenInteractive::Fullscreen()),
            renderer(ftxui::Renderer(input.input_with_submit, [&] {
                return build_layout(regs, logs, command_history, input.input_with_submit->Render());
            }))
        {}
        ~Output();

        void init();

        ftxui::ScreenInteractive screen;
        ftxui::Component renderer;

    private:
        std::vector<std::string> logs;
        std::vector<std::string> command_history;
        RegisterSnapshot regs{ 0, 0, 0x140001000, 0xffff0000 };
};