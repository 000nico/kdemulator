#pragma once
#include <vector>
#include <string>
#include <functional>
#include <ftxui/component/component.hpp>
#include <ftxui/component/event.hpp>

class InputHandler {
    public:
        using CommandCallback = std::function<void(const std::string&)>;

        InputHandler() {
            input_component = ftxui::Input(&command_input, "Type a command (/ for help)...");
            input_with_submit = ftxui::CatchEvent(input_component, [&](ftxui::Event event) {
                if (event == ftxui::Event::Return && !command_input.empty()) {
                    std::string submitted = command_input;
                    command_history.push_back("> " + submitted);
                    command_input.clear();

                    if (on_command) {
                        on_command(submitted);
                    }
                    
                    return true;
                }
                return false;
            });
        }

        void set_on_command(CommandCallback cb) {
            on_command = cb;
        }

        const std::string& get_current_input() const {
            return command_input;
        }

        const std::vector<std::string>& get_command_history() const {
            return command_history;
        }

        ftxui::Component input_with_submit;

    private:
        std::string command_input;
        std::vector<std::string> command_history;
        CommandCallback on_command;
        ftxui::Component input_component;
};
