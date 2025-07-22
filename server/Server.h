#pragma once

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
    typedef SOCKET socket_t;
    #define CLOSE_SOCKET closesocket
    #define SOCKET_ERROR_CODE WSAGetLastError()
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #include <errno.h>
    typedef int socket_t;
    #define INVALID_SOCKET -1
    #define SOCKET_ERROR -1
    #define CLOSE_SOCKET close
    #define SOCKET_ERROR_CODE errno
#endif

#include <vector>
#include <memory>
#include <thread>
#include <atomic>
#include <mutex>
#include <string>
#include "Client.h"

// Forward declaration
struct GamePacket;

class Server {
private:
    socket_t listenSocket;
    int port;
    std::atomic<bool> running;
    std::vector<std::shared_ptr<Client>> clients;
    std::mutex clientsMutex;
    std::thread acceptThread;
    
    void acceptClients();
    void handleClient(std::shared_ptr<Client> client);
    void removeClient(std::shared_ptr<Client> client);
    
    // Packet handling methods
    void handleStringPacket(std::shared_ptr<Client> client, const std::string& message);
    void handleUpdatePacket(std::shared_ptr<Client> client, const GamePacket& packet);
    
public:
    Server(int port);
    ~Server();
    
    bool initialize();
    void run();
    void runDaemon();
    void stop();
    
    void broadcastPacket(const std::vector<uint8_t>& packet, std::shared_ptr<Client> excludeClient = nullptr);
    size_t getClientCount() const;
};