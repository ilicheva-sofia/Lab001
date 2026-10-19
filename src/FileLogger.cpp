#include "FileLogger.hpp"

#include <chrono>
#include <format>

#include "DomainError.hpp"

namespace Infrastructure {

    FileLogger::FileLogger(const std::string& path) : filePath(path) {
        // Открываем файл в режиме добавления
        logFile.open(path, std::ios::app);
        if (!logFile.is_open()) {
            throw Exceptions::DomainError(
                std::format("Не удалось открыть файл лога: {}", path));
        }
    }

    FileLogger::~FileLogger() {
        if (logFile.is_open()) {
            logFile.flush();
            logFile.close();
        }
    }

    void FileLogger::log(const std::string& message) {
        if (logFile.is_open() && !message.empty()) {
            auto now = std::chrono::system_clock::now();

            std::string formatted_line =
                std::format("{:%Y-%m-%d %H:%M:%S} | {}\n", now, message);

            logFile << formatted_line;
        }
    }

}  // namespace Infrastructure
