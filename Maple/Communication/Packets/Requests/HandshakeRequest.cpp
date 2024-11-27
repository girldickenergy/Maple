#include "HandshakeRequest.h"

#include "json.hpp"
#include "VirtualizerSDK.h"
#include "xorstr.hpp"

#include "../PacketType.h"
#include "../../Crypto/CryptoProvider.h"
#include "../../../Utilities/Strings/StringUtilities.h"

#include "../../../Logging/Logger.h"

std::vector<unsigned char> HandshakeRequest::Serialize()
{
	VIRTUALIZER_SHARK_BLACK_START

	std::time_t epoch;
	std::time(&epoch);

	nlohmann::json jsonPayload;
	jsonPayload[xorstr_("Epoch")] = epoch;

	std::vector payload(CryptoProvider::Get().XOR(StringUtilities::StringToByteArray(jsonPayload.dump())));

	std::vector<unsigned char> packet;
	packet.push_back(static_cast<unsigned char>(PacketType::Handshake));
	packet.insert(packet.end(), payload.begin(), payload.end());

	VIRTUALIZER_SHARK_BLACK_END

	return packet;
}
