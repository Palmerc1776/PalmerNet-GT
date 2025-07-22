#include "Packet.h"
#include <cstring>

std::vector<uint8_t> PacketBuilder::createStringPacket(const std::string& str) {
    std::vector<uint8_t> packet;
    
    // Packet type
    packet.push_back(static_cast<uint8_t>(PacketType::STRING_PACKET));
    packet.push_back(0); // padding
    packet.push_back(0); // padding
    packet.push_back(0); // padding
    
    // String length
    uint32_t length = static_cast<uint32_t>(str.length());
    packet.insert(packet.end(), reinterpret_cast<uint8_t*>(&length), 
                  reinterpret_cast<uint8_t*>(&length) + sizeof(length));
    
    // String data
    packet.insert(packet.end(), str.begin(), str.end());
    
    return packet;
}

std::vector<uint8_t> PacketBuilder::createUpdatePacket(const GamePacket& gamePacket) {
    std::vector<uint8_t> packet;
    
    // Basic packet structure (56 bytes)
    packet.resize(56);
    
    packet[0] = static_cast<uint8_t>(gamePacket.type);
    packet[1] = gamePacket.objtype;
    packet[2] = gamePacket.count1;
    packet[3] = gamePacket.count2;
    
    // Copy the rest of the packet data
    std::memcpy(&packet[4], &gamePacket.netid, sizeof(gamePacket.netid));
    std::memcpy(&packet[8], &gamePacket.item, sizeof(gamePacket.item));
    std::memcpy(&packet[12], &gamePacket.flags, sizeof(gamePacket.flags));
    std::memcpy(&packet[16], &gamePacket.float_var, sizeof(gamePacket.float_var));
    std::memcpy(&packet[20], &gamePacket.int_data, sizeof(gamePacket.int_data));
    std::memcpy(&packet[24], &gamePacket.vec_x, sizeof(gamePacket.vec_x));
    std::memcpy(&packet[28], &gamePacket.vec_y, sizeof(gamePacket.vec_y));
    std::memcpy(&packet[32], &gamePacket.vec2_x, sizeof(gamePacket.vec2_x));
    std::memcpy(&packet[36], &gamePacket.vec2_y, sizeof(gamePacket.vec2_y));
    std::memcpy(&packet[40], &gamePacket.particle_time, sizeof(gamePacket.particle_time));
    std::memcpy(&packet[44], &gamePacket.state, sizeof(gamePacket.state));
    std::memcpy(&packet[48], &gamePacket.object_change_type, sizeof(gamePacket.object_change_type));
    packet[52] = gamePacket.particle_alt_id;
    
    // Add variable data if present
    if (!gamePacket.data.empty()) {
        packet.insert(packet.end(), gamePacket.data.begin(), gamePacket.data.end());
    }
    
    return packet;
}

GamePacket PacketBuilder::parsePacket(const std::vector<uint8_t>& data) {
    GamePacket packet;
    
    if (data.size() < 4) {
        return packet; // Invalid packet
    }
    
    packet.type = static_cast<PacketType>(data[0]);
    
    if (packet.type == PacketType::STRING_PACKET) {
        if (data.size() >= 8) {
            uint32_t length;
            std::memcpy(&length, &data[4], sizeof(length));
            
            if (data.size() >= 8 + length) {
                std::string str(data.begin() + 8, data.begin() + 8 + length);
                packet.data.assign(str.begin(), str.end());
            }
        }
    } else if (packet.type == PacketType::UPDATE_PACKET && data.size() >= 56) {
        packet.objtype = data[1];
        packet.count1 = data[2];
        packet.count2 = data[3];
        
        std::memcpy(&packet.netid, &data[4], sizeof(packet.netid));
        std::memcpy(&packet.item, &data[8], sizeof(packet.item));
        std::memcpy(&packet.flags, &data[12], sizeof(packet.flags));
        std::memcpy(&packet.float_var, &data[16], sizeof(packet.float_var));
        std::memcpy(&packet.int_data, &data[20], sizeof(packet.int_data));
        std::memcpy(&packet.vec_x, &data[24], sizeof(packet.vec_x));
        std::memcpy(&packet.vec_y, &data[28], sizeof(packet.vec_y));
        std::memcpy(&packet.vec2_x, &data[32], sizeof(packet.vec2_x));
        std::memcpy(&packet.vec2_y, &data[36], sizeof(packet.vec2_y));
        std::memcpy(&packet.particle_time, &data[40], sizeof(packet.particle_time));
        std::memcpy(&packet.state, &data[44], sizeof(packet.state));
        std::memcpy(&packet.object_change_type, &data[48], sizeof(packet.object_change_type));
        packet.particle_alt_id = data[52];
        
        // Copy any additional data
        if (data.size() > 56) {
            packet.data.assign(data.begin() + 56, data.end());
        }
    }
    
    return packet;
}

std::vector<uint8_t> PacketBuilder::createLoginResponse(bool success, const std::string& message) {
    std::string response = std::string("action|log\nmsg|") + (success ? "Login successful!" : "Login failed!") + 
                          (message.empty() ? "" : "\n" + message);
    return createStringPacket(response);
}

std::vector<uint8_t> PacketBuilder::createWorldData(const std::string& worldName) {
    std::string worldData = "action|join_request\nname|" + worldName + "\ninviteonly|0\nignorePvP|0";
    return createStringPacket(worldData);
}

std::vector<uint8_t> PacketBuilder::createPlayerData(uint32_t netid, const std::string& name, int x, int y) {
    GamePacket packet;
    packet.type = PacketType::UPDATE_PACKET;
    packet.objtype = 0; // Player spawn
    packet.netid = netid;
    packet.vec_x = static_cast<float>(x);
    packet.vec_y = static_cast<float>(y);
    
    // Add player name as data
    packet.data.assign(name.begin(), name.end());
    packet.data.push_back(0); // null terminator
    
    return createUpdatePacket(packet);
}