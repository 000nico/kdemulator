#include "src/out/panels.hpp"
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <windows.h>

using namespace ftxui;

int main() {
    SetConsoleTitle("kdemulator - github.com/000nico");
    auto screen = ScreenInteractive::Fullscreen();

    std::string command_input;
    std::vector<std::string> command_history;
    std::vector<std::string> logs = {
        "[INFO] Driver loaded at 0x140000000",
        "[WARN] Unresolved import: KeBugCheck",
    };

    RegisterSnapshot regs{ 0, 0, 0x140001000, 0xffff0000 };

    auto input_component = Input(&command_input, "Type a command...");

    auto input_with_submit = CatchEvent(input_component, [&](Event event) {
        if (event == Event::Return && !command_input.empty()) {
            command_history.push_back("> " + command_input);

            // TODO: process the command here (step, run, regs, mem, etc.)

            command_input.clear();
            return true;
        }
        return false;
    });

    auto renderer = Renderer(input_with_submit, [&] {
        return build_layout(regs, logs, command_history, input_with_submit->Render());
    });

    screen.Loop(renderer);
    return 0;
}
