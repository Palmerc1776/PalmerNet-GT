#include "Logger.h"

std::mutex Logger::logMutex;
std::ofstream Logger::logFile;
bool Logger::fileLoggingEnabled = false;

void Logger::enableFileLogging(const std::string& filename) {
    std::lock_guard<std::mutex> lock(logMutex);
    if (logFile.is_open()) {
        logFile.close();
    }
    
    logFile.open(filename, std::ios::app);
    fileLoggingEnabled = logFile.is_open();
    
    if (fileLoggingEnabled) {
        logFile << "\n=== Server Started at " << getCurrentTime() << " ===\n";
        logFile.flush();
    }
}

void Logger::disableFileLogging() {
    std::lock_guard<std::mutex> lock(logMutex);
    if (logFile.is_open()) {
        logFile.close();
    }
    fileLoggingEnabled = false;
}

std::string Logger::getCurrentTime() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    ss << '.' << std::setfill('0') << std::setw(3) << ms.count();
    
    return ss.str();
}

std::string Logger::levelToString(LogLevel level) {
    switch (level) {
        case LogLevel::INFO:    return "INFO";
        case LogLevel::WARNING: return "WARN";
        case LogLevel::ERROR:   return "ERROR";
        case LogLevel::DEBUG:   return "DEBUG";
        default:                return "UNKNOWN";
    }
}

void Logger::writeLog(LogLevel level, const std::string& message) {
    std::lock_guard<std::mutex> lock(logMutex);
    
    std::string timestamp = getCurrentTime();
    std::string levelStr = levelToString(level);
    std::string logMessage = "[" + timestamp + "] [" + levelStr + "] " + message;
    
    // Console output with colors
    switch (level) {
        case LogLevel::INFO:
            std::cout << "\033[32m" << logMessage << "\033[0m" << std::endl; // Green
            break;
        case LogLevel::WARNING:
            std::cout << "\033[33m" << logMessage << "\033[0m" << std::endl; // Yellow
            break;
        case LogLevel::ERROR:
            std::cerr << "\033[31m" << logMessage << "\033[0m" << std::endl; // Red
            break;
        case LogLevel::DEBUG:
            std::cout << "\033[36m" << logMessage << "\033[0m" << std::endl; // Cyan
            break;
    }
    
    // File output
    if (fileLoggingEnabled && logFile.is_open()) {
        logFile << logMessage << std::endl;
        logFile.flush();
    }
}

void Logger::info(const std::string& message) {
    writeLog(LogLevel::INFO, message);
}

void Logger::warning(const std::string& message) {
    writeLog(LogLevel::WARNING, message);
}

void Logger::error(const std::string& message) {
    writeLog(LogLevel::ERROR, message);
}

void Logger::debug(const std::string& message) {
    writeLog(LogLevel::DEBUG, message);
}