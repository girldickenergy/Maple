#pragma once
#include <cstdint>
#include <unordered_map>
#include <any>
#include "Fnv1a.h"
#include "ISerializeable.h"

#define PACKET_REFLECTABLE(...) Fields() = { __VA_ARGS__ };

class Packet : public ISerializeable
{
public:
    explicit Packet() = default;
    virtual const std::string GetName() = 0;

    std::unordered_map<uint32_t, FieldInfo>& Fields()
    {
        return GetFields();
    }
};