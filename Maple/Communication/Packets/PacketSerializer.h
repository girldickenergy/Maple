#pragma once
#include <Singleton.h>
#include <vector>
#include <any>
#include <unordered_map>
#include <functional>
#include "Fnv1a.h"
#include <xorstr.hpp>
#include "Packet.h"

#include <Windows.h>

class PacketSerializer : public Singleton<PacketSerializer>
{
#define ADD_RANGE(vector, data) vector.insert(vector.end(), data.begin(), data.end())

	template <typename T, typename std::enable_if<std::is_arithmetic<T>::value>::type* = nullptr>
	std::vector<uint8_t> SplitValueToBytes(const T& value)
	{
		std::vector<uint8_t> bytes;
		for (size_t i = 0; i < sizeof(value); i++)
		{
			uint8_t byte = value >> (i * 8);
			bytes.insert(bytes.begin(), byte);
		}
		return bytes;
	}

	std::vector<uint8_t> SerializeFunction(Packet& packet, const FieldInfo& fieldInfo);

	std::vector<uint8_t> SerializeType(Packet& packet);
	std::vector<uint8_t> SerializeValue(Packet& packet, const FieldInfo& fieldInfo);
public:
	explicit PacketSerializer(singletonLock);

	template <typename T, typename std::enable_if<std::is_base_of<Packet, T>::value, std::vector<uint8_t>>::type>
	std::vector<uint8_t> Serialize(const T& packet)
	{
		auto dynamicPacket = dynamic_cast<Packet*>(&packet);
		std::string packetName = dynamicPacket->GetName();

		uint32_t packetIdentifier = Hash32Fnv1a(packetName.c_str(), packetName.size());
		std::vector<uint8_t> identifier = SplitValueToBytes(packetIdentifier);

		std::vector<uint8_t> ticks = SplitValueToBytes(GetTickCount());

		std::vector<uint8_t> serializedType = SerializeType(dynamicPacket);
		auto serializedPacket = std::vector<uint8_t>();
		ADD_RANGE(serializedPacket, identifier);
		ADD_RANGE(serializedPacket, ticks);
		ADD_RANGE(serializedPacket, serializedType);

		return serializedPacket;
	}
};