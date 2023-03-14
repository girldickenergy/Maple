#include "AuthStreamStageTwoRequest.h" 

#include <ThemidaSDK.h> 
#include <json.hpp> 

#include "xorstr.hpp"
#include "../../Crypto/CryptoProvider.h" 
#include "../PacketType.h" 
#include "../../../Utilities/Strings/StringUtilities.h" 

AuthStreamStageTwoRequest::AuthStreamStageTwoRequest(const std::string& username, const std::string& checksum, const std::vector<uint8_t>& authBytes, const std::vector<uint8_t>& osuBytes)
{
	this->username = username;
	this->checksum = checksum;
	this->authBytes = authBytes;
	this->osuBytes = osuBytes;
}

std::vector<unsigned char> AuthStreamStageTwoRequest::Serialize()
{
	VM_SHARK_BLACK_START
	STR_ENCRYPT_START

	nlohmann::json jsonPayload;
	jsonPayload[xorstr_("a")] = username;
	jsonPayload[xorstr_("b")] = checksum;
	jsonPayload[xorstr_("c")] = CryptoProvider::GetInstance()->Base64Encode(authBytes);
	jsonPayload[xorstr_("d")] = CryptoProvider::GetInstance()->Base64Encode(osuBytes);

	std::vector payload(CryptoProvider::GetInstance()->AESEncrypt(StringUtilities::StringToByteArray(jsonPayload.dump())));

	std::vector<unsigned char> packet;
	packet.push_back(static_cast<unsigned char>(PacketType::AuthStreamStageTwo));
	packet.insert(packet.end(), payload.begin(), payload.end());

	STR_ENCRYPT_END
	VM_SHARK_BLACK_END

	return packet;
}
