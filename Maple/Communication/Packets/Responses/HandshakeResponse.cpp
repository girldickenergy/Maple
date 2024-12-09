#include "HandshakeResponse.h"
#include "VirtualizerSDK.h"

HandshakeResponse::HandshakeResponse()
{
	VIRTUALIZER_FISH_RED_START

	entt::meta<HandshakeResponse>().type(GetIdentifier())
		.data<&HandshakeResponse::m_Key>(Hash32Fnv1aConst("Key"))
		.data<&HandshakeResponse::m_EncryptedKey>(Hash32Fnv1aConst("EncryptedKey"))
		.data<&HandshakeResponse::m_EncryptedIv>(Hash32Fnv1aConst("EncryptedIv"));

	VIRTUALIZER_FISH_RED_END
}

const std::vector<uint8_t>& HandshakeResponse::GetKey()
{
	return m_Key;
}

const std::vector<uint8_t>& HandshakeResponse::GetEncryptedKey()
{
	return m_EncryptedKey;
}

const std::vector<uint8_t>& HandshakeResponse::GetEncryptedIv()
{
	return m_EncryptedIv;
}

uint32_t HandshakeResponse::GetIdentifier()
{
	return Hash32Fnv1aConst("HandshakeResponse");
}