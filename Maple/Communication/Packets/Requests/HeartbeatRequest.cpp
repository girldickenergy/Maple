#include "HeartbeatRequest.h"

#include "VirtualizerSDK.h"
#include "json.hpp"
#include "xorstr.hpp"

#include "../../Crypto/CryptoProvider.h"
#include "../../../Utilities/Strings/StringUtilities.h"
#include "../PacketType.h"

#include "../../../Logging/Logger.h"

HeartbeatRequest::HeartbeatRequest(const std::string& sessionToken)
{
	this->sessionToken = sessionToken;
}

std::vector<unsigned char> HeartbeatRequest::Serialize()
{
	VIRTUALIZER_TIGER_LITE_START

	Logger::StartPerformanceCounter(xorstr_("{4221618B-FAC3-45EF-8B12-DAA05133AEB0}"));
	std::time_t epoch;
	std::time(&epoch);

	nlohmann::json jsonPayload;
	jsonPayload[xorstr_("SessionToken")] = sessionToken;
	jsonPayload[xorstr_("Epoch")] = epoch + 1; // +1 to prevent the server from thinking the epoch is wrong (an edge case when we send both handshake and heartbeat packets at the same time)

	std::vector payload(CryptoProvider::Get().AESEncrypt(StringUtilities::StringToByteArray(jsonPayload.dump())));

	std::vector<unsigned char> packet;
	packet.push_back(static_cast<unsigned char>(PacketType::Heartbeat));
	packet.insert(packet.end(), payload.begin(), payload.end());

	Logger::StopPerformanceCounter(xorstr_("{4221618B-FAC3-45EF-8B12-DAA05133AEB0}"));
	VIRTUALIZER_TIGER_LITE_END

	return packet;
}
