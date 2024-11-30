#include "AuthStreamStageOneRequest.h" 
#include <VirtualizerSDK.h> 

AuthStreamStageOneRequest::AuthStreamStageOneRequest(const std::string& checksum)
{
	VIRTUALIZER_FISH_RED_START

	entt::meta<AuthStreamStageOneRequest>().type(GetIdentifier())
		.data<&AuthStreamStageOneRequest::m_Checksum>(Hash32Fnv1aConst("Checksum"));

	m_Checksum = checksum;

	VIRTUALIZER_FISH_RED_END
}

uint32_t AuthStreamStageOneRequest::GetIdentifier()
{
	return Hash32Fnv1aConst("AuthStreamStageOneRequest");
}