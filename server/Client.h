#pragma once

#ifdef _WIN32
    #include <winsock2.h>
    typedef SOCKET socket_t;
    #define CLOSE_SOCKET closesocket
    #define SOCKET_ERROR_CODE WSAGetLastError()
#else
    #include <sys/socket.h>
    #include <unistd.h>
    #include <errno.h>
    typedef int socket_t;
    #define INVALID_SOCKET -1
    #define SOCKET_ERROR -1
    #define CLOSE_SOCKET close
    #define SOCKET_ERROR_CODE errno
#endif

#include <string>
#include <vector>
#include <atomic>
#include <mutex>

class Client {
private:
    socket_t clientSocket;
    std::string ipAddress;
    std::atomic<bool> connected;
    std::mutex sendMutex;
    
    // Player data
    std::string playerName;
    int playerID;
    int worldX, worldY;
    
public:
    Client(socket_t socket, const std::string& ip);
    ~Client();
    
    bool isConnected() const;
    void disconnect();
    
    std::vector<uint8_t> receivePacket();
    bool sendPacket(const std::vector<uint8_t>& packet);
    
    // Getters
    const std::string& getIP() const { return ipAddress; }
    const std::string& getPlayerName() const { return playerName; }
    int getPlayerID() const { return playerID; }
    
    // Setters
    void setPlayerName(const std::string& name) { playerName = name; }
    void setPlayerID(int id) { playerID = id; }
    void setPosition(int x, int y) { worldX = x; worldY = y; }
};