#pragma once

enum class PacketType : unsigned char
{
    Handshake = 0xA0,
    Heartbeat = 0xE0,
    Ping = 0xF0
};