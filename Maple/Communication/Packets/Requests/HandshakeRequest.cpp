#include "HandshakeRequest.h"
#include "VirtualizerSDK.h"

HandshakeRequest::HandshakeRequest(const std::vector<std::uint32_t>& randomJunk)
{
	VIRTUALIZER_FISH_RED_START

	entt::meta<HandshakeRequest>().type(GetIdentifier())
		.data<&HandshakeRequest::m_RandomJunk>(Hash32Fnv1aConst("RandomJunk"));

	m_RandomJunk = randomJunk;

	VIRTUALIZER_FISH_RED_END
}

uint32_t HandshakeRequest::GetIdentifier()
{
	return Hash32Fnv1aConst("HandshakeRequest");
}