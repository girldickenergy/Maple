#include "AuthStreamStageTwoRequest.h"
#include <VirtualizerSDK.h>

AuthStreamStageTwoRequest::AuthStreamStageTwoRequest(const std::string& username, const std::string& checksum, const std::vector<uint8_t>& authBytes, const std::vector<uint8_t>& osuBytes)
{
	VIRTUALIZER_FISH_RED_START

	entt::meta<AuthStreamStageTwoRequest>().type(GetIdentifier())
		.data<&AuthStreamStageTwoRequest::m_Username>(Hash32Fnv1aConst("Username"))
		.data<&AuthStreamStageTwoRequest::m_Checksum>(Hash32Fnv1aConst("Checksum"))
		.data<&AuthStreamStageTwoRequest::m_AuthBytes>(Hash32Fnv1aConst("AuthBytes"))
		.data<&AuthStreamStageTwoRequest::m_OsuBytes>(Hash32Fnv1aConst("OsuBytes"));

	m_Username = username;
	m_Checksum = checksum;
	m_AuthBytes = authBytes;
	m_OsuBytes = osuBytes;

	VIRTUALIZER_FISH_RED_END
}

uint32_t AuthStreamStageTwoRequest::GetIdentifier()
{
	return Hash32Fnv1aConst("AuthStreamStageTwoRequest");
}
