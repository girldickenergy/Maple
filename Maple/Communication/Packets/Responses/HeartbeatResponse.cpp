#include "HeartbeatResponse.h"

#include "ThemidaSDK.h"
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

#pragma optimize("", off)
HeartbeatResponse HeartbeatResponse::Deserialize(const std::vector<unsigned char>& payload)
{
	VM_SHARK_BLACK_START
	STR_ENCRYPT_START

	nlohmann::json jsonPayload = nlohmann::json::parse(StringUtilities::ByteArrayToString(CryptoProvider::GetInstance()->AESDecrypt(payload)));

	HeartbeatResponse response = HeartbeatResponse(jsonPayload[xorstr_("Result")]);

	STR_ENCRYPT_END
	VM_SHARK_BLACK_END

	return response;
}
#pragma optimize("", on)
