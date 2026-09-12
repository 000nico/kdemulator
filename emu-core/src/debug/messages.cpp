#include "Debug.hpp"
#include "LogLevel.hpp"

std::string getPrefix(LogLevel level) {
    switch(level) {
        case LOG_INFO: return "[INFO] ";
        case LOG_ERROR: return "[ERROR] ";
        case LOG_WARN: return "[WARN] ";
        default: return "[?] ";
    }
}


std::string getColor(LogLevel level) {
    switch (level) {
        case LOG_INFO:  return "\033[36m"; // cyan
        case LOG_WARN:  return "\033[33m"; // yellow
        case LOG_ERROR: return "\033[31m"; // red
    }
    return "";
}

void Debug::debug_msg(std::string msg, LogLevel level){
    std::string prefix = getPrefix(level);
    std::string color = getColor(level);
    printf("%s", (color + prefix + msg).c_str());

    // persistance
    if(this->log_file){
        fprintf(log_file, "%s%s\n", prefix.c_str(), msg.c_str());
        fflush(log_file);
    }
}
