#include "HeartbeatResponse.h"
#include "VirtualizerSDK.h"

HeartbeatResponse::HeartbeatResponse()
{
	VIRTUALIZER_FISH_RED_START

	entt::meta<HeartbeatResponse>().type(GetIdentifier())
		.data<&HeartbeatResponse::m_Result>(Hash32Fnv1aConst("Result"));

	VIRTUALIZER_FISH_RED_END
}

HeartbeatResult HeartbeatResponse::GetResult()
{
	return m_Result;
}

uint32_t HeartbeatResponse::GetIdentifier()
{
	return Hash32Fnv1aConst("HeartbeatResponse");
}
