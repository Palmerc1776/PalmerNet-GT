#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <mutex>
#include <chrono>
#include <iomanip>
#include <sstream>

enum class LogLevel {
    INFO,
    WARNING,
    ERROR,
    DEBUG
};

class Logger {
private:
    static std::mutex logMutex;
    static std::ofstream logFile;
    static bool fileLoggingEnabled;
    
    static std::string getCurrentTime();
    static std::string levelToString(LogLevel level);
    static void writeLog(LogLevel level, const std::string& message);
    
public:
    static void enableFileLogging(const std::string& filename);
    static void disableFileLogging();
    
    static void info(const std::string& message);
    static void warning(const std::string& message);
    static void error(const std::string& message);
    static void debug(const std::string& message);
};