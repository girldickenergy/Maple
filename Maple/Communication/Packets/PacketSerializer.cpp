#include "PacketSerializer.h"

#include <random>

#include "VirtualizerSDK.h" 
#include "Fnv1a.h"
#include "../Crypto/CryptoProvider.h"

std::vector<uint8_t> PacketSerializer::SerializePacket(const entt::meta_any& packet)
{
    VIRTUALIZER_FISH_RED_START

    auto identifier = packet.type().id();
    auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

    auto serializedPacket = std::vector<uint8_t>();
    auto serializedType = SerializeType(packet);

    ADD_RANGE(serializedPacket, TO_BYTE_VECTOR(identifier));
    ADD_RANGE(serializedPacket, TO_BYTE_VECTOR(timestamp));
    ADD_RANGE(serializedPacket, serializedType);

    // todo: checksum

    return serializedPacket;

    VIRTUALIZER_FISH_RED_END
}

std::vector<uint8_t> PacketSerializer::SerializeType(const entt::meta_any& instance)
{
    VIRTUALIZER_FISH_RED_START

    auto type = instance.type();

    auto serializedType = std::vector<uint8_t>();

    int fieldCount = 0;
    for (auto [identifier, field] : type.data())
    {
        auto value = field.get(instance);
        auto serializedValue = SerializeValue(value);

        ADD_RANGE(serializedType, TO_BYTE_VECTOR(identifier));
        ADD_RANGE(serializedType, serializedValue);

        fieldCount++;
    }

    ADD_RANGE_BEGIN(serializedType, TO_BYTE_VECTOR(fieldCount));

    return serializedType;

    VIRTUALIZER_FISH_RED_END
}

std::vector<uint8_t> PacketSerializer::SerializeValue(const entt::meta_any& value, bool suppressType)
{
    VIRTUALIZER_FISH_RED_START

    auto serializedValue = std::vector<uint8_t>();

    auto type = value.type();
    auto identifier = m_TypeSerializers.contains(type.id()) ? type.id() : (type.is_sequence_container() ? Hash32Fnv1aConst("Array") : Hash32Fnv1aConst("CustomType"));

    if (!suppressType)
        ADD_RANGE(serializedValue, TO_BYTE_VECTOR(identifier));

    auto epochMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

    std::mt19937 random(epochMilliseconds ^ 0xdeadbeef);
    uint32_t key1 = random();
    uint32_t key2 = random();
    uint32_t key3 = random() ^ (epochMilliseconds >> 10) ^ 0xdeadbeef;

    if (m_TypeSerializers.contains(type.id()))
    {
        auto& serializeFunction = m_TypeSerializers[type.id()];

        auto encryptedValue = CryptoProvider::Get().ApplyCryptoTransformations(serializeFunction(value), key1, key2, key3);
        int encryptedValueLength = encryptedValue.size();

        ADD_RANGE(serializedValue, TO_BYTE_VECTOR(encryptedValueLength));
        ADD_RANGE(serializedValue, TO_BYTE_VECTOR(key1));
        ADD_RANGE(serializedValue, TO_BYTE_VECTOR(key2));
        ADD_RANGE(serializedValue, TO_BYTE_VECTOR(key3));
        ADD_RANGE(serializedValue, encryptedValue);
    }
    else if (type.is_sequence_container()) // todo: array support?
    {
        auto container = value.as_sequence_container();

        auto elementType = container.value_type();
        auto elementTypeIdentifier = elementType.id();

        if (!m_TypeSerializers.contains(elementType.id()))
            elementTypeIdentifier = elementType.is_sequence_container() ? Hash32Fnv1aConst("Array") : Hash32Fnv1aConst("CustomType");

        bool isPrimitive = elementType.is_arithmetic();
        int elementCount = container.size();

        ADD_RANGE(serializedValue, TO_BYTE_VECTOR(elementTypeIdentifier));
        ADD_RANGE(serializedValue, TO_BYTE_VECTOR(isPrimitive));
        ADD_RANGE(serializedValue, TO_BYTE_VECTOR(elementCount));

        if (isPrimitive)
        {
            auto& serializeFunction = m_TypeSerializers[elementTypeIdentifier];

            auto items = std::vector<uint8_t>();
            for (auto item : container)
            {
                auto serializedItem = serializeFunction(item);
                items.insert(items.end(), serializedItem.begin(), serializedItem.end());
            }

            auto encryptedValue = CryptoProvider::Get().ApplyCryptoTransformations(items, key1, key2, key3);
            int encryptedValueLength = encryptedValue.size();

            ADD_RANGE(serializedValue, TO_BYTE_VECTOR(encryptedValueLength));
            ADD_RANGE(serializedValue, TO_BYTE_VECTOR(key1));
            ADD_RANGE(serializedValue, TO_BYTE_VECTOR(key2));
            ADD_RANGE(serializedValue, TO_BYTE_VECTOR(key3));
            ADD_RANGE(serializedValue, encryptedValue);
        }
        else
        {
            for (auto item : container)
                ADD_RANGE(serializedValue, SerializeValue(item, true));
        }
    }
    else
    {
        ADD_RANGE(serializedValue, SerializeType(value));
    }

    return serializedValue;

    VIRTUALIZER_FISH_RED_END
}

std::expected<entt::meta_any, SerializationError> PacketSerializer::DeserializePacket(uint32_t identifier, BinaryReader& reader)
{
    VIRTUALIZER_FISH_RED_START

    auto packet = entt::resolve(identifier).construct();
    if (!packet || packet.type().id() != identifier)
        return std::unexpected(SerializationError::PacketUnknown);

    auto timestamp = reader.Read<int64_t>(); // todo: do something with it

    if (const auto type = DeserializeType(packet, reader); !type.has_value())
        return std::unexpected(type.error());

    return packet;

    VIRTUALIZER_FISH_RED_END
}

std::expected<void, SerializationError> PacketSerializer::DeserializeType(entt::meta_any& instance, BinaryReader& reader)
{
    VIRTUALIZER_FISH_RED_START

    auto type = instance.type();

    auto fieldCount = reader.Read<uint32_t>();

    for (size_t i = 0; i < fieldCount; i++)
    {
        auto nameHash = reader.Read<uint32_t>();

        auto field = type.data(nameHash);
        if (!field)
            return std::unexpected(SerializationError::FieldNotFound);

        if (const auto value = DeserializeValue(field.type(), reader); value.has_value())
        {
            if (!field.set(instance, *value))
                return std::unexpected(SerializationError::FieldNotSet);
        }
        else
            return std::unexpected(value.error());
    }
    return { };

    VIRTUALIZER_FISH_RED_END
}

std::expected<entt::meta_any, SerializationError> PacketSerializer::DeserializeValue(const entt::meta_type& type, BinaryReader& reader, uint32_t typeIdentifier)
{
    VIRTUALIZER_FISH_RED_START

    if (typeIdentifier == 0)
        typeIdentifier = reader.Read<uint32_t>();

    if (m_TypeDeserializers.contains(typeIdentifier))
    {
        auto& deserializeFunction = m_TypeDeserializers[typeIdentifier];

        auto valueLength = reader.Read<int32_t>();
        auto key1 = reader.Read<uint32_t>();
        auto key2 = reader.Read<uint32_t>();
        auto key3 = reader.Read<uint32_t>();

        auto valueBuffer = reader.Read(valueLength);

        auto decryptedValue = CryptoProvider::Get().ApplyCryptoTransformations(valueBuffer, key1, key2, key3, true);
        auto value = deserializeFunction(decryptedValue);

        if (value.type().id() != type.id())
            return std::unexpected(SerializationError::TypeIdMismatch);

        return value;
    }

    if (typeIdentifier == Hash32Fnv1aConst("Array"))
    {
	    if (type.is_sequence_container()) // todo: array support?
	    {
            auto elementTypeIdentifier = reader.Read<uint32_t>();
            auto isPrimitiveType = reader.Read<bool>();
            auto arrayElementCount = reader.Read<int32_t>();

            auto constructedArray = type.construct();
            if (!constructedArray)
                return std::unexpected(SerializationError::ConstructFailed);

            auto container = constructedArray.as_sequence_container();
            auto elementType = container.value_type();

            if (!elementType)
                return std::unexpected(SerializationError::NoValueType);

            if (elementType.is_arithmetic() != isPrimitiveType)
                return std::unexpected(SerializationError::ArrayTypeMismatch);

            if (isPrimitiveType)
            {
                if (!m_TypeDeserializers.contains(elementTypeIdentifier))
                    return std::unexpected(SerializationError::TypeDeserializerNotFound);

                auto& elementDeserializer = m_TypeDeserializers[elementTypeIdentifier];

                auto arrayBufferLength = reader.Read<int32_t>();
                auto key1 = reader.Read<uint32_t>();
                auto key2 = reader.Read<uint32_t>();
                auto key3 = reader.Read<uint32_t>();

                auto arrayBuffer = reader.Read(arrayBufferLength);
                auto decryptedArrayBuffer = CryptoProvider::Get().ApplyCryptoTransformations(arrayBuffer, key1, key2, key3, true);

                auto elementSize = arrayBufferLength / arrayElementCount;

                for (int i = 0; i < arrayElementCount; i++)
                {
                    auto item = elementDeserializer(std::vector(decryptedArrayBuffer.begin() + elementSize * i, decryptedArrayBuffer.begin() + elementSize * (i + 1)));
                    container.insert(container.end(), item);
                }
            }
            else
            {
                for (int i = 0; i < arrayElementCount; i++)
                {
                    if (const auto item = DeserializeValue(elementType, reader, elementTypeIdentifier); item.has_value())
                        container.insert(container.end(), *item);
                    else
                        return std::unexpected(item.error());
                }
            }

            return constructedArray;
	    }

        return std::unexpected(SerializationError::Unknown);
    }

    if (typeIdentifier == Hash32Fnv1aConst("CustomType"))
    {
        auto instance = type.construct();
        if (!instance)
            return std::unexpected(SerializationError::CustomTypeNotFound);

        if (const auto type = DeserializeType(instance, reader); !type.has_value())
            return std::unexpected(type.error());

        return instance;
    }

    return std::unexpected(SerializationError::Unknown);

    VIRTUALIZER_FISH_RED_END
}

PacketSerializer::PacketSerializer(singletonLock)
{
    VIRTUALIZER_FISH_RED_START

    // register our own identifiers for primitive types
    entt::meta<uint8_t>().type(Hash32Fnv1aConst("Byte"));
    entt::meta<bool>().type(Hash32Fnv1aConst("Boolean"));
    entt::meta<int16_t>().type(Hash32Fnv1aConst("Int16"));
    entt::meta<uint16_t>().type(Hash32Fnv1aConst("UInt16"));
    entt::meta<int32_t>().type(Hash32Fnv1aConst("Int32"));
    entt::meta<uint32_t>().type(Hash32Fnv1aConst("UInt32"));
    entt::meta<float>().type(Hash32Fnv1aConst("Single"));
    entt::meta<int64_t>().type(Hash32Fnv1aConst("Int64"));
    entt::meta<uint64_t>().type(Hash32Fnv1aConst("UInt64"));
    entt::meta<double>().type(Hash32Fnv1aConst("Double"));
    entt::meta<std::string>().type(Hash32Fnv1aConst("String"));

    auto toByteVector = [](const entt::meta_any& value)
	{
        return std::vector(static_cast<const uint8_t*>(value.data()), static_cast<const uint8_t*>(value.data()) + value.type().size_of());
    };

    m_TypeSerializers =
    {
        { Hash32Fnv1aConst("Byte"), toByteVector },
        { Hash32Fnv1aConst("Boolean"), toByteVector },
        { Hash32Fnv1aConst("Int16"), toByteVector },
        { Hash32Fnv1aConst("UInt16"), toByteVector },
        { Hash32Fnv1aConst("Int32"), toByteVector },
        { Hash32Fnv1aConst("UInt32"), toByteVector },
        { Hash32Fnv1aConst("Single"), toByteVector },
        { Hash32Fnv1aConst("Int64"), toByteVector },
        { Hash32Fnv1aConst("UInt64"), toByteVector },
        { Hash32Fnv1aConst("Double"), toByteVector },
        {
            Hash32Fnv1aConst("String"), [](entt::meta_any value)
            {
                auto str = value.cast<std::string>();
                return std::vector<uint8_t>(str.begin(), str.end());
            }
        }
    };

    m_TypeDeserializers =
    {
        { Hash32Fnv1aConst("Byte"), [](std::vector<uint8_t> buffer) { return *buffer.data(); } },
        { Hash32Fnv1aConst("Boolean"), [](std::vector<uint8_t> buffer) { return *reinterpret_cast<bool*>(buffer.data()); } },
        { Hash32Fnv1aConst("Int16"), [](std::vector<uint8_t> buffer) { return *reinterpret_cast<int16_t*>(buffer.data()); } },
        { Hash32Fnv1aConst("UInt16"), [](std::vector<uint8_t> buffer) { return *reinterpret_cast<uint16_t*>(buffer.data()); } },
        { Hash32Fnv1aConst("Int32"), [](std::vector<uint8_t> buffer) { return *reinterpret_cast<int32_t*>(buffer.data()); } },
        { Hash32Fnv1aConst("UInt32"), [](std::vector<uint8_t> buffer) { return *reinterpret_cast<uint32_t*>(buffer.data()); } },
        { Hash32Fnv1aConst("Single"), [](std::vector<uint8_t> buffer) { return *reinterpret_cast<float*>(buffer.data()); } },
        { Hash32Fnv1aConst("Int64"), [](std::vector<uint8_t> buffer) { return *reinterpret_cast<int64_t*>(buffer.data()); } },
        { Hash32Fnv1aConst("UInt64"), [](std::vector<uint8_t> buffer) { return *reinterpret_cast<uint64_t*>(buffer.data()); } },
        { Hash32Fnv1aConst("Double"), [](std::vector<uint8_t> buffer) { return *reinterpret_cast<double*>(buffer.data()); } },
        { Hash32Fnv1aConst("String"),[](std::vector<uint8_t> buffer) { return std::string(buffer.begin(), buffer.end()); } }
    };

    VIRTUALIZER_FISH_RED_END
}

std::expected<std::pair<entt::meta_any, uint32_t>, SerializationError> PacketSerializer::Deserialize(const std::vector<uint8_t>& buffer)
{
    VIRTUALIZER_FISH_RED_START

    BinaryReader reader(buffer);

    auto identifier = reader.Read<uint32_t>();

    if (entt::resolve(identifier).id() != identifier)
        return std::unexpected(SerializationError::IdentifierUnknown);

    if (const auto packet = DeserializePacket(identifier, reader); packet.has_value())
        return std::make_pair(packet, identifier);
    else
        return std::unexpected(packet.error());

    VIRTUALIZER_FISH_RED_END
}