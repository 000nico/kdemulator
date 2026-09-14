#pragma once
#include <string>
#include <functional>
#include <vector>
#include <cstdint>

#include "LogLevel.hpp"

struct TraceEntry {
    uint64_t address;
    std::string instruction;
    std::string formatted;
};

using LogCallback = std::function<void(const std::string&, LogLevel)>;
using TraceCallback = std::function<void(const TraceEntry&)>;

class Debug {
    public:
        static void set_callback(LogCallback cb);
        static void set_trace_callback(TraceCallback cb);
        static void init(const std::string& log_path);
        static void shutdown();
        static void debug_msg(const std::string& msg, LogLevel level);

        static void add_trace(uint64_t address, const std::string& instruction);
        static const std::vector<TraceEntry>& get_trace_history();
        static void clear_trace_history();

        static inline bool traceMode = false;

    private:
        static inline FILE*                   log_file = nullptr;
        static inline LogCallback             s_callback = nullptr;
        static inline TraceCallback           s_trace_callback = nullptr;
        static inline std::vector<TraceEntry> s_trace_history;
};
