#include "Client.h"
#include "../utils/Logger.h"
#include <cstring>

Client::Client(socket_t socket, const std::string& ip) 
    : clientSocket(socket), ipAddress(ip), connected(true), playerID(-1), worldX(0), worldY(0) {
}

Client::~Client() {
    disconnect();
}

bool Client::isConnected() const {
    return connected;
}

void Client::disconnect() {
    if (connected) {
        connected = false;
        if (clientSocket != INVALID_SOCKET) {
            CLOSE_SOCKET(clientSocket);
            clientSocket = INVALID_SOCKET;
        }
    }
}

std::vector<uint8_t> Client::receivePacket() {
    if (!connected) {
        return {};
    }
    
    // First, receive the packet length (4 bytes)
    uint32_t packetLength = 0;
    int bytesReceived = recv(clientSocket, (char*)&packetLength, sizeof(packetLength), 0);
    
    if (bytesReceived <= 0) {
        if (bytesReceived == 0) {
            Logger::info("Client " + ipAddress + " disconnected gracefully");
        } else {
            Logger::error("Error receiving packet length from " + ipAddress + ". Error: " + std::to_string(SOCKET_ERROR_CODE));
        }
        disconnect();
        return {};
    }
    
    // Convert from network byte order if needed
    // packetLength = ntohl(packetLength);
    
    // Sanity check for packet length
    if (packetLength > 1024 * 1024) { // 1MB max
        Logger::error("Packet too large from " + ipAddress + ": " + std::to_string(packetLength));
        disconnect();
        return {};
    }
    
    // Receive the actual packet data
    std::vector<uint8_t> packet(packetLength);
    size_t totalReceived = 0;
    
    while (totalReceived < packetLength) {
        int received = recv(clientSocket, (char*)packet.data() + totalReceived, 
                           packetLength - totalReceived, 0);
        
        if (received <= 0) {
            Logger::error("Error receiving packet data from " + ipAddress);
            disconnect();
            return {};
        }
        
        totalReceived += received;
    }
    
    return packet;
}

bool Client::sendPacket(const std::vector<uint8_t>& packet) {
    if (!connected || packet.empty()) {
        return false;
    }
    
    std::lock_guard<std::mutex> lock(sendMutex);
    
    // Send packet length first
    uint32_t packetLength = static_cast<uint32_t>(packet.size());
    // uint32_t networkLength = htonl(packetLength);
    
    int bytesSent = send(clientSocket, (char*)&packetLength, sizeof(packetLength), 0);
    if (bytesSent != sizeof(packetLength)) {
        Logger::error("Failed to send packet length to " + ipAddress);
        disconnect();
        return false;
    }
    
    // Send packet data
    size_t totalSent = 0;
    while (totalSent < packet.size()) {
        int sent = send(clientSocket, (char*)packet.data() + totalSent, 
                       packet.size() - totalSent, 0);
        
        if (sent <= 0) {
            Logger::error("Failed to send packet data to " + ipAddress);
            disconnect();
            return false;
        }
        
        totalSent += sent;
    }
    
    return true;
}