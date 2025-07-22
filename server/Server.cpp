#include "Server.h"
#include "../utils/Logger.h"
#include "../protocol/Packet.h"
#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <ctime>

Server::Server(int port) : listenSocket(INVALID_SOCKET), port(port), running(false) {
#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        Logger::error("WSAStartup failed");
    }
#endif
    // Seed random number generator for player IDs
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
}

Server::~Server() {
    stop();
#ifdef _WIN32
    WSACleanup();
#endif
}

bool Server::initialize() {
    // Create socket
    listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listenSocket == INVALID_SOCKET) {
        Logger::error("Failed to create socket. Error: " + std::to_string(SOCKET_ERROR_CODE));
        return false;
    }

    // Set socket options
    int opt = 1;
#ifdef _WIN32
    if (setsockopt(listenSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt)) == SOCKET_ERROR) {
#else
    if (setsockopt(listenSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == SOCKET_ERROR) {
#endif
        Logger::warning("Failed to set SO_REUSEADDR");
    }

    // Bind socket
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

    if (bind(listenSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        Logger::error("Failed to bind socket. Error: " + std::to_string(SOCKET_ERROR_CODE));
        CLOSE_SOCKET(listenSocket);
        return false;
    }

    // Listen for connections
    if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR) {
        Logger::error("Failed to listen on socket. Error: " + std::to_string(SOCKET_ERROR_CODE));
        CLOSE_SOCKET(listenSocket);
        return false;
    }

    return true;
}

void Server::run() {
    running = true;
    
    // Start accept thread
    acceptThread = std::thread(&Server::acceptClients, this);
    
    Logger::info("Server is running. Press Enter to stop...");
    std::cin.get();
    
    stop();
}

void Server::stop() {
    if (!running) return;
    
    running = false;
    
    // Close listen socket
    if (listenSocket != INVALID_SOCKET) {
        CLOSE_SOCKET(listenSocket);
        listenSocket = INVALID_SOCKET;
    }
    
    // Wait for accept thread to finish
    if (acceptThread.joinable()) {
        acceptThread.join();
    }
    
    // Disconnect all clients
    std::lock_guard<std::mutex> lock(clientsMutex);
    for (auto& client : clients) {
        client->disconnect();
    }
    clients.clear();
    
    Logger::info("Server stopped");
}

void Server::acceptClients() {
    while (running) {
        sockaddr_in clientAddr{};
        socklen_t clientAddrLen = sizeof(clientAddr);
        
        socket_t clientSocket = accept(listenSocket, (sockaddr*)&clientAddr, &clientAddrLen);
        
        if (clientSocket == INVALID_SOCKET) {
            if (running) {
                Logger::error("Failed to accept client connection. Error: " + std::to_string(SOCKET_ERROR_CODE));
            }
            continue;
        }
        
        // Get client IP
        char clientIP[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &clientAddr.sin_addr, clientIP, INET_ADDRSTRLEN);
        
        Logger::info("New client connected from: " + std::string(clientIP));
        
        // Create client object
        auto client = std::make_shared<Client>(clientSocket, std::string(clientIP));
        
        // Add to clients list
        {
            std::lock_guard<std::mutex> lock(clientsMutex);
            clients.push_back(client);
        }
        
        // Start handling client in separate thread
        std::thread clientThread(&Server::handleClient, this, client);
        clientThread.detach();
    }
}

void Server::handleClient(std::shared_ptr<Client> client) {
    Logger::info("Handling client: " + client->getIP());
    
    // Send welcome message
    auto welcomePacket = PacketBuilder::createStringPacket("action|log\nmsg|Welcome to Growtopia Private Server!");
    client->sendPacket(welcomePacket);
    
    while (running && client->isConnected()) {
        auto packetData = client->receivePacket();
        if (packetData.empty()) {
            break; // Client disconnected or error
        }
        
        // Parse the packet
        GamePacket packet = PacketBuilder::parsePacket(packetData);
        
        // Handle different packet types
        if (packet.type == PacketType::STRING_PACKET) {
            std::string message(packet.data.begin(), packet.data.end());
            Logger::info("Received string packet from " + client->getIP() + ": " + message);
            
            // Handle login requests, world joins, etc.
            handleStringPacket(client, message);
        }
        else if (packet.type == PacketType::UPDATE_PACKET) {
            Logger::debug("Received update packet from " + client->getIP());
            // Handle player movement, actions, etc.
            handleUpdatePacket(client, packet);
        }
        else {
            Logger::debug("Received unknown packet type from " + client->getIP());
        }
    }
    
    Logger::info("Client disconnected: " + client->getIP());
    removeClient(client);
}

void Server::removeClient(std::shared_ptr<Client> client) {
    std::lock_guard<std::mutex> lock(clientsMutex);
    clients.erase(std::remove(clients.begin(), clients.end(), client), clients.end());
}

void Server::broadcastPacket(const std::vector<uint8_t>& packet, std::shared_ptr<Client> excludeClient) {
    std::lock_guard<std::mutex> lock(clientsMutex);
    for (auto& client : clients) {
        if (client != excludeClient && client->isConnected()) {
            client->sendPacket(packet);
        }
    }
}

size_t Server::getClientCount() const {
    std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(clientsMutex));
    return clients.size();
}

void Server::handleStringPacket(std::shared_ptr<Client> client, const std::string& message) {
    Logger::debug("Processing string packet: " + message);
    
    // Parse action-based messages (Growtopia protocol)
    if (message.find("action|") == 0) {
        size_t actionEnd = message.find('\n');
        std::string action = message.substr(7, actionEnd - 7); // Skip "action|"
        
        if (action == "login") {
            // Handle login request
            Logger::info("Login request from " + client->getIP());
            
            // For now, accept all logins
            auto response = PacketBuilder::createLoginResponse(true, "Welcome to the server!");
            client->sendPacket(response);
            
            // Set a default player name
            client->setPlayerName("Guest_" + std::to_string(rand() % 10000));
            client->setPlayerID(static_cast<int>(clients.size()));
            
        } else if (action == "join_request") {
            // Handle world join request
            size_t nameStart = message.find("name|");
            if (nameStart != std::string::npos) {
                size_t nameEnd = message.find('\n', nameStart);
                std::string worldName = message.substr(nameStart + 5, nameEnd - nameStart - 5);
                
                Logger::info("World join request from " + client->getIP() + " for world: " + worldName);
                
                // Send world data
                auto worldData = PacketBuilder::createWorldData(worldName);
                client->sendPacket(worldData);
                
                // Send player spawn data
                auto playerData = PacketBuilder::createPlayerData(client->getPlayerID(), 
                                                                client->getPlayerName(), 
                                                                100, 100); // Default spawn position
                client->sendPacket(playerData);
            }
        } else if (action == "quit") {
            Logger::info("Client " + client->getIP() + " requested disconnect");
            client->disconnect();
        } else {
            Logger::debug("Unknown action: " + action);
        }
    } else {
        // Handle other string messages (chat, etc.)
        Logger::info("Chat message from " + client->getIP() + ": " + message);
        
        // Broadcast chat message to all clients
        auto chatPacket = PacketBuilder::createStringPacket("action|log\nmsg|" + 
                                                           client->getPlayerName() + ": " + message);
        broadcastPacket(chatPacket, client);
    }
}

void Server::handleUpdatePacket(std::shared_ptr<Client> client, const GamePacket& packet) {
    // Handle player movement, block placement, etc.
    Logger::debug("Update packet from " + client->getIP() + 
                 " - Type: " + std::to_string(static_cast<int>(packet.objtype)) +
                 " - NetID: " + std::to_string(packet.netid));
    
    // For now, just broadcast the update to all other clients
    auto updateData = PacketBuilder::createUpdatePacket(packet);
    broadcastPacket(updateData, client);
}