#include "PingResponse.h"
#include "VirtualizerSDK.h"

PingResponse::PingResponse()
{
	VIRTUALIZER_FISH_RED_START

	entt::meta<PingResponse>().type(GetIdentifier());

	VIRTUALIZER_FISH_RED_END
}

uint32_t PingResponse::GetIdentifier()
{
	return Hash32Fnv1aConst("PingResponse");
}
