#include "AuthStreamStageOneRequest.h" 

#include <ThemidaSDK.h> 
#include <json.hpp> 

#include "xorstr.hpp"
#include "../../Crypto/CryptoProvider.h" 
#include "../PacketType.h" 
#include "../../../Utilities/Strings/StringUtilities.h" 

AuthStreamStageOneRequest::AuthStreamStageOneRequest(const std::string& checksum)
{
	this->checksum = checksum;
}

std::vector<unsigned char> AuthStreamStageOneRequest::Serialize()
{
	VM_SHARK_BLACK_START
	STR_ENCRYPT_START
		
	nlohmann::json jsonPayload;
	jsonPayload[xorstr_("a")] = checksum;

	std::vector payload(CryptoProvider::Get().AESEncrypt(StringUtilities::StringToByteArray(jsonPayload.dump())));

	std::vector<unsigned char> packet;
	packet.push_back(static_cast<unsigned char>(PacketType::AuthStreamStageOne));
	packet.insert(packet.end(), payload.begin(), payload.end());

	STR_ENCRYPT_END
	VM_SHARK_BLACK_END

	return packet;
}
