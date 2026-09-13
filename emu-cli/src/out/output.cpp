#include "Output.hpp"
#include "../../../emu-core/src/debug/Debug.hpp"
#include <windows.h>

void Output::init(){
    SetConsoleTitle("kdemulator - github.com/000nico");

    Debug::init("kdemulator.log");

    Debug::set_callback([this](const std::string& msg, LogLevel) {
        logs.push_back(msg);
        screen.PostEvent(ftxui::Event::Custom);
    });
}

Output::~Output() {
    Debug::set_callback(nullptr);
    Debug::shutdown();
}
