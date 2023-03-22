#include "HeartbeatResponse.h"

#include "VirtualizerSDK.h"
#include "json.hpp"
#include "xorstr.hpp"
#include "../../Crypto/CryptoProvider.h"
#include "../../../Utilities/Strings/StringUtilities.h"

HeartbeatResponse::HeartbeatResponse(HeartbeatResult result)
{
	this->result = result;
}

HeartbeatResult HeartbeatResponse::GetResult()
{
	return result;
}

HeartbeatResponse HeartbeatResponse::Deserialize(const std::vector<unsigned char>& payload)
{
	VIRTUALIZER_SHARK_BLACK_START
	
	nlohmann::json jsonPayload = nlohmann::json::parse(StringUtilities::ByteArrayToString(CryptoProvider::Get().AESDecrypt(payload)));

	HeartbeatResponse response = HeartbeatResponse(jsonPayload[xorstr_("Result")]);

		VIRTUALIZER_SHARK_BLACK_END

	return response;
}
