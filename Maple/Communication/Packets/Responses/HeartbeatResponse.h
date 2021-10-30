#pragma once

#include "Response.h"

enum class HeartbeatResult : unsigned char
{
	Success = 0x0,
	InvalidSession = 0x2,
	EpochTimedOut = 0x3,
	InternalError = 0x4
};

class HeartbeatResponse : public Response
{
public:
	HeartbeatResult Result;
	HeartbeatResponse(const char* msg, size_t size, MatchedClient* matchedClient);
};