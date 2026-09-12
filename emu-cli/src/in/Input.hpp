#pragma once
#include <vector>
#include <string>
#include <ftxui/component/component.hpp>
#include <ftxui/component/event.hpp>

class InputHandler {
    public:
        InputHandler() {
            input_component = ftxui::Input(&command_input, "Type a command...");
            input_with_submit = ftxui::CatchEvent(input_component, [&](ftxui::Event event) {
                if (event == ftxui::Event::Return && !command_input.empty()) {
                    command_history.push_back("> " + command_input);

                    // TODO: process the command here (step, run, regs, mem, etc.)

                    command_input.clear();
                    return true;
                }
                return false;
            });
        }

        ftxui::Component input_with_submit;

    private:
        std::string command_input;
        std::vector<std::string> command_history;
        ftxui::Component input_component;
};