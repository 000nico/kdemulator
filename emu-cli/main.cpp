#include "src/out/Output.hpp"
#include "src/in/Input.hpp"
#include <windows.h>

int main() {
    InputHandler input;
    Output output(input);
    
    output.init();

    output.screen.Loop(output.renderer);
    return 0;
}
