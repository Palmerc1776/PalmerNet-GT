#include <iostream>
#include <thread>
#include <vector>
#include <memory>
#include "server/Server.h"
#include "utils/Logger.h"

int main() {
    try {
        Logger::info("=== Growtopia Private Server ===");
        Logger::info("Version: 1.0.0");
        Logger::info("Cross-platform C++ implementation");
        Logger::info("================================");
        
        // Enable file logging
        Logger::enableFileLogging("server.log");
        Logger::info("File logging enabled: server.log");
        
        // Initialize server on port 17091 (default Growtopia port)
        Server server(17091);
        
        if (!server.initialize()) {
            Logger::error("Failed to initialize server");
            return -1;
        }
        
        Logger::info("Server initialized successfully");
        Logger::info("Server listening on port 17091");
        Logger::info("Ready to accept client connections");
        Logger::info("Press Enter to stop the server...");
        
        // Start server
        server.run();
        
        Logger::info("Server shutdown complete");
        
    } catch (const std::exception& e) {
        Logger::error("Server error: " + std::string(e.what()));
        return -1;
    }
    
    return 0;
}