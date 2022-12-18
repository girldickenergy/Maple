#include "HandshakeRequest.h"

#include "json.hpp"
#include "ThemidaSDK.h"

#include "../PacketType.h"
#include "../../Crypto/CryptoProvider.h"
#include "../../../Utilities/Strings/StringUtilities.h"
#include "../../../Utilities/Security/xorstr.hpp"

#pragma optimize("", off)
std::vector<unsigned char> HandshakeRequest::Serialize()
{
	VM_SHARK_BLACK_START
	STR_ENCRYPT_START

	std::time_t epoch;
	std::time(&epoch);

	nlohmann::json jsonPayload;
	jsonPayload[xorstr_("Epoch")] = epoch;

	std::vector payload(CryptoProvider::GetInstance()->XOR(StringUtilities::StringToByteArray(jsonPayload.dump())));

	std::vector<unsigned char> packet;
	packet.push_back(static_cast<unsigned char>(PacketType::Handshake));
	packet.insert(packet.end(), payload.begin(), payload.end());

	STR_ENCRYPT_END
	VM_SHARK_BLACK_END

	return packet;
}
#pragma optimize("", on)
