#include "PacketSerializer.h"
#include "../Crypto/CryptoProvider.h"
#include <random>

std::vector<uint8_t> PacketSerializer::SerializeFunction(Packet& packet, const FieldInfo& fieldInfo)
{
	auto serializedType = std::vector<uint8_t>();

	auto instance = reinterpret_cast<void*>(&packet);
	if (fieldInfo.TypeId == typeid(int32_t).hash_code())
		return SplitValueToBytes(fieldInfo.Get<int32_t>(instance));
	else if (fieldInfo.TypeId == typeid(uint32_t).hash_code())
		return SplitValueToBytes(fieldInfo.Get<uint32_t>(instance));
	else if (fieldInfo.TypeId == typeid(int64_t).hash_code())
		return SplitValueToBytes(fieldInfo.Get<int64_t>(instance));
	else if (fieldInfo.TypeId == typeid(uint64_t).hash_code())
		return SplitValueToBytes(fieldInfo.Get<uint64_t>(instance));
	else if (fieldInfo.TypeId == typeid(uint64_t).hash_code())
		return SplitValueToBytes(fieldInfo.Get<uint64_t>(instance));
	else if (fieldInfo.TypeId == typeid(float_t).hash_code())
		return SplitValueToBytes(static_cast<uint32_t>(fieldInfo.Get<float_t>(instance)));
	else if (fieldInfo.TypeId == typeid(double_t).hash_code())
		return SplitValueToBytes(static_cast<uint64_t>(fieldInfo.Get<double_t>(instance)));
	else if (fieldInfo.TypeId == typeid(short).hash_code())
		return SplitValueToBytes(fieldInfo.Get<short>(instance));
	else if (fieldInfo.TypeId == typeid(bool).hash_code())
		return SplitValueToBytes(fieldInfo.Get<bool>(instance));
	else if (fieldInfo.TypeId == typeid(char).hash_code())
		return SplitValueToBytes(fieldInfo.Get<char>(instance));
	else if (fieldInfo.TypeId == typeid(uint8_t).hash_code())
		return SplitValueToBytes(fieldInfo.Get<uint8_t>(instance));
	else if (fieldInfo.TypeId == typeid(std::string).hash_code())
	{
		auto& string = fieldInfo.Get<std::string>(instance);
		std::vector<uint8_t> bytes(string.begin(), string.end());
		bytes.push_back('\0');
		return bytes;
	}

	return { };
}

std::vector<uint8_t> PacketSerializer::SerializeType(Packet& packet)
{
	auto serializedPacket = std::vector<uint8_t>();
	std::unordered_map<uint32_t, FieldInfo>& fields = packet.GetFields();
	std::vector<uint8_t> amountOfFields = SplitValueToBytes(fields.size());

	ADD_RANGE(serializedPacket, amountOfFields);
	for (auto const& field : fields)
	{
		std::vector<uint8_t> hashedName = SplitValueToBytes(field.first);
		ADD_RANGE(serializedPacket, hashedName);

		std::vector<uint8_t> serializedValue = SerializeValue(packet, field.second);
		ADD_RANGE(serializedPacket, serializedValue);
	}

	return serializedPacket;
}

std::vector<uint8_t> PacketSerializer::SerializeValue(Packet& packet, const FieldInfo& fieldInfo)
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

PacketSerializer::PacketSerializer(singletonLock)
{
}
