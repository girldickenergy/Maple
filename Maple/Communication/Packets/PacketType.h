#pragma once

enum class PacketType : unsigned char
{
    Handshake = 0xA0,
    AuthStreamStageOne = 0xD2,
    AuthStreamStageTwo = 0xD3,
    Heartbeat = 0xE0,
    Ping = 0xF0
};