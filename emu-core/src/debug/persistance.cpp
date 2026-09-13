#include "Debug.hpp"

void Debug::init(const std::string& log_path) {
    if (log_file) {
        fclose(log_file);
        log_file = nullptr;
    }
    log_file = fopen(log_path.c_str(), "w");
    if (!log_file) {
        printf("cant open log file: %s\n", log_path.c_str());
    }
}

void Debug::shutdown(){
    if (log_file) {
        fclose(log_file);
        log_file = nullptr;
    }
}