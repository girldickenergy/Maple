#pragma once

#include "Request.h"

#include "../../MatchedClient.h"

class HeartbeatRequest : public Request
{
public:
	HeartbeatRequest(std::string sessionID, MatchedClient* matchedClient);
};
