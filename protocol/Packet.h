#pragma once

#include <vector>
#include <string>
#include <cstdint>

// Growtopia packet types
enum class PacketType : uint8_t {
    UNKNOWN = 0,
    STRING_PACKET = 2,
    INTEGER_PACKET = 3,
    FLOAT_PACKET = 4,
    COMPOUND_PACKET = 5,
    UPDATE_PACKET = 6
};

// Basic packet structure
struct GamePacket {
    PacketType type;
    uint8_t objtype;
    uint8_t count1;
    uint8_t count2;
    uint32_t netid;
    uint32_t item;
    uint32_t flags;
    float float_var;
    uint32_t int_data;
    float vec_x;
    float vec_y;
    float vec2_x;
    float vec2_y;
    float particle_time;
    uint32_t state;
    uint32_t object_change_type;
    uint8_t particle_alt_id;
    
    // Variable length data
    std::vector<uint8_t> data;
    
    GamePacket() : type(PacketType::UNKNOWN), objtype(0), count1(0), count2(0),
                   netid(0), item(0), flags(0), float_var(0.0f), int_data(0),
                   vec_x(0.0f), vec_y(0.0f), vec2_x(0.0f), vec2_y(0.0f),
                   particle_time(0.0f), state(0), object_change_type(0),
                   particle_alt_id(0) {}
};

class PacketBuilder {
public:
    static std::vector<uint8_t> createStringPacket(const std::string& str);
    static std::vector<uint8_t> createUpdatePacket(const GamePacket& packet);
    static GamePacket parsePacket(const std::vector<uint8_t>& data);
    
    // Helper functions for common packets
    static std::vector<uint8_t> createLoginResponse(bool success, const std::string& message = "");
    static std::vector<uint8_t> createWorldData(const std::string& worldName);
    static std::vector<uint8_t> createPlayerData(uint32_t netid, const std::string& name, int x, int y);
};