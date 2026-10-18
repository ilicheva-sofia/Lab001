#include "FileLogger.hpp"

#include <chrono>
#include <format>
#include <iomanip>

#include "DomainError.hpp"

Infrastructure::FileLogger::FileLogger(const std::string& path)
    : filePath(path) {
    // Открываем файл в режиме добавления
    logFile.open(path, std::ios::app);

    if (!logFile.is_open()) {
        throw Exceptions::FileLoggerError(path);
    }
}

Infrastructure::FileLogger::~FileLogger() {
    if (logFile.is_open()) {
        logFile.flush();
        logFile.close();
    }
}

void Infrastructure::FileLogger::log(const std::string& message) {
    if (logFile.is_open() && !message.empty()) {
        auto now = std::chrono::system_clock::now();
        auto now_c = std::chrono::system_clock::to_time_t(now);

        std::tm ptm = *std::localtime(&now_c);
        std::stringstream ss;
        ss << std::put_time(&ptm, "%Y-%m-%d %H:%M:%S") << " | " << message
           << std::endl;

        logFile << ss.str();
    }
}
