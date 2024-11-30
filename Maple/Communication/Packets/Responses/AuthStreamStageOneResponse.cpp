#include "AuthStreamStageOneResponse.h" 
#include "VirtualizerSDK.h" 

AuthStreamStageOneResponse::AuthStreamStageOneResponse()
{
	VIRTUALIZER_FISH_RED_START

	entt::meta<AuthStreamStageOneResponse>().type(GetIdentifier())
		.data<&AuthStreamStageOneResponse::m_ShouldSend>(Hash32Fnv1aConst("ShouldSend"));

	VIRTUALIZER_FISH_RED_END
}

bool AuthStreamStageOneResponse::GetShouldSend()
{
	return m_ShouldSend;
}

uint32_t AuthStreamStageOneResponse::GetIdentifier()
{
	return Hash32Fnv1aConst("AuthStreamStageOneResponse");
}