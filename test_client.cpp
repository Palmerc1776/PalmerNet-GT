#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <chrono>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
    typedef SOCKET socket_t;
    #define CLOSE_SOCKET closesocket
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    typedef int socket_t;
    #define INVALID_SOCKET -1
    #define CLOSE_SOCKET close
#endif

class TestClient {
private:
    socket_t clientSocket;
    bool connected;
    
public:
    TestClient() : clientSocket(INVALID_SOCKET), connected(false) {
#ifdef _WIN32
        WSADATA wsaData;
        WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif
    }
    
    ~TestClient() {
        disconnect();
#ifdef _WIN32
        WSACleanup();
#endif
    }
    
    bool connect(const std::string& host, int port) {
        clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (clientSocket == INVALID_SOCKET) {
            std::cerr << "Failed to create socket" << std::endl;
            return false;
        }
        
        sockaddr_in serverAddr{};
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(port);
        inet_pton(AF_INET, host.c_str(), &serverAddr.sin_addr);
        
        if (::connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) != 0) {
            std::cerr << "Failed to connect to server" << std::endl;
            CLOSE_SOCKET(clientSocket);
            return false;
        }
        
        connected = true;
        std::cout << "Connected to server at " << host << ":" << port << std::endl;
        return true;
    }
    
    void disconnect() {
        if (connected) {
            connected = false;
            CLOSE_SOCKET(clientSocket);
        }
    }
    
    bool sendPacket(const std::vector<uint8_t>& packet) {
        if (!connected) return false;
        
        uint32_t length = static_cast<uint32_t>(packet.size());
        
        // Send length
        if (send(clientSocket, (char*)&length, sizeof(length), 0) != sizeof(length)) {
            return false;
        }
        
        // Send data
        size_t totalSent = 0;
        while (totalSent < packet.size()) {
            int sent = send(clientSocket, (char*)packet.data() + totalSent, 
                           packet.size() - totalSent, 0);
            if (sent <= 0) return false;
            totalSent += sent;
        }
        
        return true;
    }
    
    std::vector<uint8_t> receivePacket() {
        if (!connected) return {};
        
        uint32_t length;
        if (recv(clientSocket, (char*)&length, sizeof(length), 0) != sizeof(length)) {
            return {};
        }
        
        std::vector<uint8_t> packet(length);
        size_t totalReceived = 0;
        
        while (totalReceived < length) {
            int received = recv(clientSocket, (char*)packet.data() + totalReceived,
                               length - totalReceived, 0);
            if (received <= 0) return {};
            totalReceived += received;
        }
        
        return packet;
    }
    
    std::vector<uint8_t> createStringPacket(const std::string& str) {
        std::vector<uint8_t> packet;
        packet.push_back(2); // STRING_PACKET
        packet.push_back(0); // padding
        packet.push_back(0); // padding
        packet.push_back(0); // padding
        
        uint32_t length = static_cast<uint32_t>(str.length());
        packet.insert(packet.end(), reinterpret_cast<uint8_t*>(&length),
                      reinterpret_cast<uint8_t*>(&length) + sizeof(length));
        
        packet.insert(packet.end(), str.begin(), str.end());
        return packet;
    }
};

int main() {
    TestClient client;
    
    if (!client.connect("127.0.0.1", 17091)) {
        std::cerr << "Failed to connect to server" << std::endl;
        return -1;
    }
    
    // Receive welcome message
    auto welcomePacket = client.receivePacket();
    if (!welcomePacket.empty()) {
        std::string welcome(welcomePacket.begin() + 8, welcomePacket.end());
        std::cout << "Server says: " << welcome << std::endl;
    }
    
    // Send login request
    std::cout << "Sending login request..." << std::endl;
    auto loginPacket = client.createStringPacket("action|login\nusername|TestUser\npassword|test123");
    client.sendPacket(loginPacket);
    
    // Receive login response
    auto loginResponse = client.receivePacket();
    if (!loginResponse.empty()) {
        std::string response(loginResponse.begin() + 8, loginResponse.end());
        std::cout << "Login response: " << response << std::endl;
    }
    
    // Send world join request
    std::cout << "Sending world join request..." << std::endl;
    auto worldPacket = client.createStringPacket("action|join_request\nname|START");
    client.sendPacket(worldPacket);
    
    // Receive world data
    auto worldResponse = client.receivePacket();
    if (!worldResponse.empty()) {
        std::string response(worldResponse.begin() + 8, worldResponse.end());
        std::cout << "World response: " << response << std::endl;
    }
    
    // Send a chat message
    std::cout << "Sending chat message..." << std::endl;
    auto chatPacket = client.createStringPacket("Hello from test client!");
    client.sendPacket(chatPacket);
    
    // Wait a bit then disconnect
    std::this_thread::sleep_for(std::chrono::seconds(2));
    
    std::cout << "Disconnecting..." << std::endl;
    client.disconnect();
    
    return 0;
}