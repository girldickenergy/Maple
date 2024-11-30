#include "HeartbeatRequest.h"
#include "VirtualizerSDK.h"

HeartbeatRequest::HeartbeatRequest(const std::string& sessionToken)
{
	VIRTUALIZER_FISH_RED_START

	entt::meta<HeartbeatRequest>().type(GetIdentifier())
		.data<&HeartbeatRequest::m_SessionToken>(Hash32Fnv1aConst("SessionToken"));

	m_SessionToken = sessionToken;

	VIRTUALIZER_FISH_RED_END
}

uint32_t HeartbeatRequest::GetIdentifier()
{
	return Hash32Fnv1aConst("HeartbeatRequest");
}
