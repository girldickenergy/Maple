#pragma once
#include <cstdint>

struct CRC
{
    uint32_t FunctionName;          // 0x00
    uint32_t FunctionNameXORKey;    // 0x04
    uint32_t ClassName;             // 0x08
    uint32_t ClassNameXORKey;       // 0x0C
    uint32_t Unk1;                  // 0x10
    uint32_t Unk1XORKey;            // 0x14
    uint32_t Unk2;                  // 0x18
    uint32_t Unk2XORKey;            // 0x1C
    uint32_t FunctionPointer;       // 0x20
    uint32_t FunctionPointerXORKey; // 0x24
    uint32_t FunctionSize;          // 0x28
    uint32_t FunctionSizeXORKey;    // 0x2C
    uint32_t Checksum;              // 0x30
    uint32_t ChecksumXORKey;        // 0x34
    uint32_t Unk3;                  // 0x38
    uint32_t Unk3XORKey;            // 0x3C
};
