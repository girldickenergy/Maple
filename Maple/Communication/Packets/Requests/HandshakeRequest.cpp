#include "HandshakeRequest.h"
#include "VirtualizerSDK.h"

HandshakeRequest::HandshakeRequest()
{
	VIRTUALIZER_FISH_RED_START

	entt::meta<HandshakeRequest>().type(GetIdentifier())
		.data<&HandshakeRequest::m_Epoch>(Hash32Fnv1aConst("Epoch"));

	std::time(&m_Epoch);

	VIRTUALIZER_FISH_RED_END
}

uint32_t HandshakeRequest::GetIdentifier()
{
	return Hash32Fnv1aConst("HandshakeRequest");
}