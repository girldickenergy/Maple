#include "AuthStreamStageOneResponse.h" 

#include "ThemidaSDK.h" 
#include "json.hpp" 
#include "../../Crypto/CryptoProvider.h" 
#include "../../../Utilities/Strings/StringUtilities.h" 
#include "../../../Utilities/Security/xorstr.hpp" 

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
	VM_SHARK_BLACK_START
	STR_ENCRYPT_START

	nlohmann::json jsonPayload = nlohmann::json::parse(StringUtilities::ByteArrayToString(CryptoProvider::GetInstance()->AESDecrypt(payload)));

	AuthStreamStageOneResponse response = AuthStreamStageOneResponse(jsonPayload[xorstr_("a")]);

	STR_ENCRYPT_END
	VM_SHARK_BLACK_END

	return response;
}
