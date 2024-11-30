#pragma once

#include <functional>
#include <unordered_map>
#include <vector>
#include <expected>

#include "entt.hpp"
#include "Singleton.h"

#include "IPacket.h"
#include "BinaryReader.h"
#include "SerializationError.h"

class PacketSerializer : public Singleton<PacketSerializer>
{
	#define TO_BYTE_VECTOR(data) [&]() { \
	    std::vector<uint8_t> bytes; \
	    for (size_t i = 0; i < sizeof(data); ++i) { \
	        bytes.push_back((data >> (i * 8)) & 0xFF); \
	    } \
	    return bytes; \
	}()

	#define ADD_RANGE(vector, data) \
	    do { \
	        auto tempVec = data; \
	        (vector).insert((vector).end(), tempVec.begin(), tempVec.end()); \
	    } while (0)

	#define ADD_RANGE_BEGIN(vector, data) \
	    do { \
	        auto tempVec = data; \
	        (vector).insert((vector).begin(), tempVec.begin(), tempVec.end()); \
	    } while (0)

	std::unordered_map<uint32_t, std::function<std::vector<uint8_t>(entt::meta_any)>> m_TypeSerializers;
	std::unordered_map<uint32_t, std::function<entt::meta_any(std::vector<uint8_t>)>> m_TypeDeserializers;

	std::vector<uint8_t> SerializePacket(const entt::meta_any& packet);
	std::vector<uint8_t> SerializeType(const entt::meta_any& instance);
	std::vector<uint8_t> SerializeValue(const entt::meta_any& value, bool suppressType = false);
    std::expected<entt::meta_any, SerializationError> DeserializePacket(uint32_t identifier, BinaryReader& reader);
	std::expected<void, SerializationError> DeserializeType(entt::meta_any& instance, BinaryReader& reader);
    std::expected<entt::meta_any, SerializationError> DeserializeValue(const entt::meta_type& type, BinaryReader& reader, uint32_t typeIdentifier = 0);

public:
	explicit PacketSerializer(singletonLock);

	template <typename T, std::enable_if_t<std::is_base_of_v<IPacket, T>>* = nullptr>
	std::expected<std::vector<uint8_t>, SerializationError> Serialize(T& packet)
	{
		auto identifier = dynamic_cast<IPacket*>(&packet)->GetIdentifier();

		if (entt::resolve(identifier).id() != identifier)
			return std::unexpected(SerializationError::IdentifierUnknown);

		return SerializePacket(packet);
	}

	std::expected<std::pair<entt::meta_any, uint32_t>, SerializationError> Deserialize(const std::vector<uint8_t>& buffer);
};
