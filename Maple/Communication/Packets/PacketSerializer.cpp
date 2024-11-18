#include "PacketSerializer.h"
#include "Fnv1a.h"
#include <xorstr.hpp>

PacketSerializer::PacketSerializer(singletonLock)
{
	m_TypeSerializers.emplace(Hash32Fnv1aConst("Int32"),
		[this](const std::any& object) {
			return SplitValueToBytes(std::any_cast<int32_t>(object));
		}
	);
	m_TypeSerializers.emplace(Hash32Fnv1aConst("UInt32"),
		[this](const std::any& object) {
			return SplitValueToBytes(std::any_cast<uint32_t>(object));
		}
	);
	m_TypeSerializers.emplace(Hash32Fnv1aConst("Int64"),
		[this](const std::any& object) {
			return SplitValueToBytes(std::any_cast<int64_t>(object));
		}
	);
	m_TypeSerializers.emplace(Hash32Fnv1aConst("UInt64"),
		[this](const std::any& object) {
			return SplitValueToBytes(std::any_cast<uint64_t>(object));
		}
	);
	m_TypeSerializers.emplace(Hash32Fnv1aConst("Single"),
		[this](const std::any& object) {
			return SplitValueToBytes(static_cast<uint32_t>(std::any_cast<float_t>(object)));
		}
	);
	m_TypeSerializers.emplace(Hash32Fnv1aConst("Double"),
		[this](const std::any& object) {
			return SplitValueToBytes(static_cast<uint64_t>(std::any_cast<float_t>(object)));
		}
	);
	m_TypeSerializers.emplace(Hash32Fnv1aConst("Short"),
		[this](const std::any& object) {
			return SplitValueToBytes(std::any_cast<short>(object));
		}
	);
	m_TypeSerializers.emplace(Hash32Fnv1aConst("Bool"),
		[this](const std::any& object) {
			return SplitValueToBytes(std::any_cast<bool>(object));
		}
	);
	m_TypeSerializers.emplace(Hash32Fnv1aConst("Char"),
		[this](const std::any& object) {
			return SplitValueToBytes(std::any_cast<uint8_t>(object));
		}
	);
	m_TypeSerializers.emplace(Hash32Fnv1aConst("Byte"),
		[](const std::any& object) {
			return std::vector<uint8_t>{ std::any_cast<uint8_t>(object) };
		}
	);
	m_TypeSerializers.emplace(Hash32Fnv1aConst("String"),
		[](const std::any& object) {
			auto string = std::any_cast<std::string>(object);
			std::vector<uint8_t> bytes(string.begin(), string.end());
			bytes.push_back('\0');
			return bytes;
		}
	);
}
