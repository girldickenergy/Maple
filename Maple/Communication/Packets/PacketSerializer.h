#pragma once
#include <Singleton.h>
#include "../Crypto/CryptoProvider.h"
#include <vector>
#include <any>
#include <unordered_map>
#include <functional>
#include "Fnv1a.h"
#include <xorstr.hpp>
#include "Packet.h"
#include <random>

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

	template <typename T, typename std::enable_if<std::is_base_of<ISerializeable, T>::value>::type* = nullptr>
	std::vector<uint8_t> SerializeType(T& type)
	{
		auto serializedPacket = std::vector<uint8_t>();
		std::unordered_map<uint32_t, FieldInfo>& fields = type.GetFields();
		std::vector<uint8_t> amountOfFields = SplitValueToBytes(fields.size());

		ADD_RANGE(serializedPacket, amountOfFields);
		for (auto const& field : fields)
		{
			std::vector<uint8_t> hashedName = SplitValueToBytes(field.first);
			ADD_RANGE(serializedPacket, hashedName);

			std::vector<uint8_t> serializedValue = SerializeValue(type, field.second);
			ADD_RANGE(serializedPacket, serializedValue);
		}

		return serializedPacket;
	}

	template <typename T, typename std::enable_if<std::is_base_of<ISerializeable, T>::value>::type* = nullptr>
	std::vector<uint8_t> SerializeValue(T& packet, const FieldInfo& fieldInfo)
	{
		auto serializedValue = std::vector<uint8_t>();

		std::vector<uint8_t> serializedField = SerializeFunction(packet, fieldInfo);
		if (!serializedField.empty())
		{
			const std::string& packetName = packet.GetName();
			std::vector<uint8_t> typeIdentifier = SplitValueToBytes(Hash32Fnv1a(packetName.c_str(), packetName.size()));
			ADD_RANGE(serializedValue, typeIdentifier);

			auto random = std::mt19937();
			uint32_t key1 = random();
			uint32_t key2 = random();
			uint32_t key3 = (GetTickCount() >> 10) ^ 0xdeadbeef;
			std::vector<uint8_t> serializedKey1 = SplitValueToBytes(key1);
			std::vector<uint8_t> serializedKey2 = SplitValueToBytes(key2);
			std::vector<uint8_t> serializedKey3 = SplitValueToBytes(key3);

			std::vector<uint8_t> encryptedValue = CryptoProvider::Get().ApplyCryptoTransformations(serializedField, key1, key2, key3);

			std::vector<uint8_t> length = SplitValueToBytes(encryptedValue.size());

			ADD_RANGE(serializedValue, length);

			ADD_RANGE(serializedValue, serializedKey1);
			ADD_RANGE(serializedValue, serializedKey2);
			ADD_RANGE(serializedValue, serializedKey3);

			ADD_RANGE(serializedValue, encryptedValue);
		} //TODO: implement arrays and vectors
		else
		{
			std::vector<uint8_t> serializedType = SerializeType(packet);
			ADD_RANGE(serializedValue, serializedType);
		}

		return serializedValue;
	}
public:
	explicit PacketSerializer(singletonLock);

	template <typename T, typename std::enable_if<std::is_base_of<Packet, T>::value>::type* = nullptr>
	std::vector<uint8_t> Serialize(T& packet)
	{
		auto dynamicPacket = dynamic_cast<Packet*>(&packet);
		std::string packetName = dynamicPacket->GetName();

		uint32_t packetIdentifier = Hash32Fnv1a(packetName.c_str(), packetName.size());
		std::vector<uint8_t> identifier = SplitValueToBytes(packetIdentifier);

		std::vector<uint8_t> ticks = SplitValueToBytes(GetTickCount());

		std::vector<uint8_t> serializedType = SerializeType(packet);
		auto serializedPacket = std::vector<uint8_t>();
		ADD_RANGE(serializedPacket, identifier);
		ADD_RANGE(serializedPacket, ticks);
		ADD_RANGE(serializedPacket, serializedType);

		return serializedPacket;
	}
};