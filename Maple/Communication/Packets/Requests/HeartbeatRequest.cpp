#include "HeartbeatRequest.h"

#include "../../../Utilities/Strings/StringUtilities.h"

HeartbeatRequest::HeartbeatRequest(std::string sessionID, MatchedClient* matchedClient) : Request(RequestType::Heartbeat)
{
	AddString(matchedClient->aes->Encrypt(StringUtilities::StringToByteArray(sessionID)));
}
