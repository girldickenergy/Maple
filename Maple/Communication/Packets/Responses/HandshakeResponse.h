#pragma once

#include "Response.h"

enum class HandshakeResult : unsigned char
{
	Success = 0x0,
	EpochTimedOut = 0x1,
	InternalError = 0x2
};

class HandshakeResponse : public Response
{
public:
	HandshakeResult Result;
	std::vector<unsigned char> IV;
	std::vector<unsigned char> Key;

	HandshakeResponse(const char* msg, size_t size);
};
