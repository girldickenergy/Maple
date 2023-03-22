#include "AuthStreamStageOneResponse.h" 

#include "VirtualizerSDK.h" 
#include "json.hpp" 
#include "xorstr.hpp"
#include "../../Crypto/CryptoProvider.h" 
#include "../../../Utilities/Strings/StringUtilities.h" 

AuthStreamStageOneResponse::AuthStreamStageOneResponse(bool shouldSend)
{
	this->shouldSend = shouldSend;
}

bool AuthStreamStageOneResponse::GetShouldSend()
{
	return shouldSend;
}

AuthStreamStageOneResponse AuthStreamStageOneResponse::Deserialize(const std::vector<unsigned char>& payload)
{
	VIRTUALIZER_SHARK_BLACK_START
	
	nlohmann::json jsonPayload = nlohmann::json::parse(StringUtilities::ByteArrayToString(CryptoProvider::Get().AESDecrypt(payload)));

	AuthStreamStageOneResponse response = AuthStreamStageOneResponse(jsonPayload[xorstr_("a")]);

		VIRTUALIZER_SHARK_BLACK_END

	return response;
}
