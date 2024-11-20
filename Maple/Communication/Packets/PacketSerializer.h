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
		std::reverse(bytes.begin(), bytes.end());
		return bytes;
	}

	std::pair<std::string, std::vector<uint8_t>> SerializeFunction(Packet& packet, const FieldInfo& fieldInfo);

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

		std::pair<std::string, std::vector<uint8_t>> serializedField = SerializeFunction(packet, fieldInfo);
		if (!serializedField.second.empty())
		{
			std::vector<uint8_t> typeIdentifier = SplitValueToBytes(Hash32Fnv1a(serializedField.first.c_str(), serializedField.first.size()));
			ADD_RANGE(serializedValue, typeIdentifier);

			auto random = std::mt19937();
			uint32_t key1 = random();
			uint32_t key2 = random();
			auto milliSeconds = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
			uint32_t key3 = (milliSeconds >> 10) ^ 0xdeadbeef;
			std::vector<uint8_t> serializedKey1 = SplitValueToBytes(key1);
			std::vector<uint8_t> serializedKey2 = SplitValueToBytes(key2);
			std::vector<uint8_t> serializedKey3 = SplitValueToBytes(key3);

			std::vector<uint8_t> encryptedValue = CryptoProvider::Get().ApplyCryptoTransformations(serializedField.second, key1, key2, key3);

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

		auto milliSeconds = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		std::vector<uint8_t> ticks = SplitValueToBytes(milliSeconds);

		std::vector<uint8_t> serializedType = SerializeType(packet);
		auto serializedPacket = std::vector<uint8_t>();
		ADD_RANGE(serializedPacket, identifier);
		ADD_RANGE(serializedPacket, ticks);
		ADD_RANGE(serializedPacket, serializedType);

		return serializedPacket;
	}
};