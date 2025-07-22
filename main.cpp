#include <iostream>
#include <thread>
#include <vector>
#include <memory>
#include <csignal>
#include <atomic>
#include "server/Server.h"
#include "utils/Logger.h"

// Global flag for graceful shutdown
std::atomic<bool> shutdownRequested(false);
Server* globalServer = nullptr;

// Signal handler for graceful shutdown
void signalHandler(int signal) {
    if (signal == SIGINT || signal == SIGTERM) {
        Logger::info("Shutdown signal received, stopping server...");
        shutdownRequested = true;
        if (globalServer) {
            globalServer->stop();
        }
    }
}

int main(int argc, char* argv[]) {
    try {
        Logger::info("=== Growtopia Private Server ===");
        Logger::info("Version: 1.0.0");
        Logger::info("Cross-platform C++ implementation");
        Logger::info("================================");
        
        // Enable file logging
        Logger::enableFileLogging("server.log");
        Logger::info("File logging enabled: server.log");
        
        // Check if running in daemon mode (no arguments = daemon mode)
        bool daemonMode = (argc == 1);
        
        // Set up signal handlers for graceful shutdown
        std::signal(SIGINT, signalHandler);
        std::signal(SIGTERM, signalHandler);
        
        // Initialize server on port 17091 (default Growtopia port)
        Server server(17091);
        globalServer = &server;
        
        if (!server.initialize()) {
            Logger::error("Failed to initialize server");
            return -1;
        }
        
        Logger::info("Server initialized successfully");
        Logger::info("Server listening on port 17091");
        Logger::info("Ready to accept client connections");
        
        if (daemonMode) {
            Logger::info("Running in daemon mode (use SIGTERM to stop)");
            // Start server in daemon mode
            server.runDaemon();
        } else {
            Logger::info("Press Enter to stop the server...");
            // Start server in interactive mode
            server.run();
        }
        
        Logger::info("Server shutdown complete");
        
    } catch (const std::exception& e) {
        Logger::error("Server error: " + std::string(e.what()));
        return -1;
    }
    
    return 0;
}