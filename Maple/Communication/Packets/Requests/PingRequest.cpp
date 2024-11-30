#include "PingRequest.h"
#include "VirtualizerSDK.h"

PingRequest::PingRequest()
{
	VIRTUALIZER_FISH_RED_START

	entt::meta<PingRequest>().type(GetIdentifier());

	VIRTUALIZER_FISH_RED_END
}

uint32_t PingRequest::GetIdentifier()
{
	return Hash32Fnv1aConst("PingRequest");
}
