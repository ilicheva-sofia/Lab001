#include <string>
#include <vector>

#include "Logger.hpp"

namespace Infrastructure {
    class MemoryLogger : public Ports::Logger {
       private:
        std::vector<std::string> logs;

       public:
        void log(const std::string& message) override {
            logs.push_back(message);
        }
        ~MemoryLogger() { logs.clear(); }
    };
}  // namespace Infrastructure