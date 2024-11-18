#pragma once
#include <Singleton.h>
#include <vector>
#include <any>
#include <unordered_map>
#include <functional>
#include "Packet.h"
#include "Fnv1a.h"
#include <xorstr.hpp>
#include <sysinfoapi.h>

class PacketSerializer : public Singleton<PacketSerializer>
{
	std::unordered_map<uint32_t, std::function<std::vector<uint8_t>(const std::any&)>> m_TypeSerializers;

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

		auto serializedPacket = std::vector<uint8_t>();
		serializedPacket.insert(serializedPacket.end(), identifier.begin(), identifier.end());
		serializedPacket.insert(serializedPacket.end(), ticks.begin(), ticks.end());
	}
};