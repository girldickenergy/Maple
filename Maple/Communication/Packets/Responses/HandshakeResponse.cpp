#include "HandshakeResponse.h"
#include "VirtualizerSDK.h"

HandshakeResponse::HandshakeResponse()
{
	VIRTUALIZER_FISH_RED_START

	entt::meta<HandshakeResponse>().type(GetIdentifier())
		.data<&HandshakeResponse::m_Key>(Hash32Fnv1aConst("Key"))
		.data<&HandshakeResponse::m_Iv>(Hash32Fnv1aConst("Iv"));

	VIRTUALIZER_FISH_RED_END
}

const std::vector<uint8_t>& HandshakeResponse::GetKey()
{
	return m_Key;
}

const std::vector<uint8_t>& HandshakeResponse::GetIV()
{
	return m_Iv;
}

uint32_t HandshakeResponse::GetIdentifier()
{
	return Hash32Fnv1aConst("HandshakeResponse");
}