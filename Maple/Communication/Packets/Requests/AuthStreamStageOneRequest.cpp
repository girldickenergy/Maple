#include "AuthStreamStageOneRequest.h" 

#include <VirtualizerSDK.h> 
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
	VIRTUALIZER_SHARK_BLACK_START
			
	nlohmann::json jsonPayload;
	jsonPayload[xorstr_("a")] = checksum;

	std::vector payload(CryptoProvider::Get().AESEncrypt(StringUtilities::StringToByteArray(jsonPayload.dump())));

	std::vector<unsigned char> packet;
	packet.push_back(static_cast<unsigned char>(PacketType::AuthStreamStageOne));
	packet.insert(packet.end(), payload.begin(), payload.end());

		VIRTUALIZER_SHARK_BLACK_END

	return packet;
}
