#pragma once
#include <fstream>
#include <string>

#include "Logger.hpp"

namespace Infrastructure {

    class FileLogger : public Ports::Logger {
       private:
        std::ofstream logFile;
        std::string filePath;

       public:
        explicit FileLogger(const std::string& path);

        // Деструктор для корректного закрытия файла
        ~FileLogger() override;

        void log(const std::string& message) override;
    };

}  // namespace Infrastructure
