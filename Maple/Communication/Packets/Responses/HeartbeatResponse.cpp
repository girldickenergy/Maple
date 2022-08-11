#include "HeartbeatResponse.h"

#include "../../../Utilities/Strings/StringUtilities.h"

#include <chrono>

HeartbeatResponse::HeartbeatResponse(const char* msg, size_t size, MatchedClient* matchedClient) : Response(msg, size)
{
	VM_SHARK_BLACK_START
	std::chrono::milliseconds msEpoch = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());

	auto encrypted = StringUtilities::StringToByteArray(RawData[0]);

	encrypted.erase(encrypted.begin());

	std::string decrypted = matchedClient->aes->Decrypt(encrypted);

	std::vector<std::string> decryptedSplit = StringUtilities::Split(decrypted);
	Result = static_cast<HeartbeatResult>(decryptedSplit[0][0]);
	decryptedSplit[1].erase(decryptedSplit[1].begin());
	std::string epoch = decryptedSplit[1];

	if (std::abs(msEpoch.count() - ((std::stoll(epoch) * 2) ^ 0xDA)) > 5000)
	{
		Result = HeartbeatResult::EpochTimedOut;
		return;
	}
	VM_SHARK_BLACK_END
}
