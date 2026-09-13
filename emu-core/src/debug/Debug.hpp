#pragma once
#include <string>
#include <functional>

#include "LogLevel.hpp"

using LogCallback = std::function<void(const std::string&, LogLevel)>;

class Debug {
    public:
        static void set_callback(LogCallback cb);
        static void init(const std::string& log_path);
        static void shutdown();
        static void debug_msg(const std::string& msg, LogLevel level);

    private:
        static inline FILE*        log_file = nullptr;
        static inline LogCallback  s_callback = nullptr;
};
