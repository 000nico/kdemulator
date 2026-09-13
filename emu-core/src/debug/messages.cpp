#include "Debug.hpp"
#include "LogLevel.hpp"
#include <ctime>

std::string getTimestamp() {
    time_t now = time(nullptr);
    char buf[32];
    strftime(buf, sizeof(buf), "%H:%M:%S", localtime(&now));
    return std::string(buf);
}


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

void Debug::set_callback(LogCallback cb) {
    s_callback = cb;
}

void Debug::debug_msg(const std::string& msg, LogLevel level){
    std::string prefix = getPrefix(level);
    std::string color = getColor(level);
    std::string timestamp = getTimestamp();
    printf("%s", (color + prefix + msg).c_str());

    std::string cleaned_msg = msg;
    while (!cleaned_msg.empty() && (cleaned_msg.back() == '\n' || cleaned_msg.back() == '\r')) {
        cleaned_msg.pop_back();
    }

    // persistance
    if(log_file){
        fprintf(log_file, "[%s] %s%s\n", timestamp.c_str(), prefix.c_str(), cleaned_msg.c_str());
        fflush(log_file);
    }

    // callback para emu-cli
    if(s_callback){
        s_callback(timestamp + " " + prefix + cleaned_msg, level);
    }
}
