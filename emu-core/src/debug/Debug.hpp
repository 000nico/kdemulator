#include <string>
#include <vector>

class Debug {
    public:
        void debug_msg(std::string msg);

    private:
        std::vector<std::string> logs;
};
