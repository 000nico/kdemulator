#include <string>

#include "LogLevel.hpp"

class Debug {
    public:
        void init(const std::string& log_path);
        void shutdown();
        void debug_msg(std::string msg, LogLevel level);

    private:
        static inline FILE* log_file = nullptr;
};
