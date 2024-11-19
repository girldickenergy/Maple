#pragma once
#include <cstdint>
#include <unordered_map>
#include <any>
#include "Fnv1a.h"

struct FieldInfo
{
    uint32_t TypeId;
    size_t Offset;

    template <typename T>
    T& Get(void* instance) const
    {
        return *reinterpret_cast<T*>(static_cast<char*>(instance) + Offset);
    }

    template <typename T>
    void Set(void* instance, const T& value) const
    {
        Get<T>(instance) = value;
    }
};

#define TYPE(name, owner, member) { Hash32Fnv1aConst(name), { typeid(member).hash_code(), offsetof(owner, member) } }
#define REFLECTABLE(...) m_Fields = __VA_ARGS__;

class Packet
{
protected:
    std::unordered_map<uint32_t, FieldInfo> m_Fields;
public:
	explicit Packet() = default;

    std::unordered_map<uint32_t, FieldInfo>& GetFields()
    {
        return m_Fields;
    }

    virtual const std::string& GetName() = 0;
};

class HeartbeatPacket : Packet
{
    int a;
public:
    HeartbeatPacket()
    {
        REFLECTABLE( { TYPE("Type", HeartbeatPacket, a) } )
    }
};
