#pragma once
#include <cstdint>

enum class SerializationError : uint8_t
{
	Unknown,
	IdentifierUnknown,
	PacketUnknown,
	FieldNotFound,
	FieldNotSet,
	TypeIdMismatch,
	ConstructFailed,
	NoValueType,
	ArrayTypeMismatch,
	TypeDeserializerNotFound,
	CustomTypeNotFound
};