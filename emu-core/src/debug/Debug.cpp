#include "Debug.hpp"

void Debug::debug_msg(std::string msg){
    this->logs.push_back(msg);
}