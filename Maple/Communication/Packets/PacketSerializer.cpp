#include "PacketSerializer.h"

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

PacketSerializer::PacketSerializer(singletonLock)
{
}
