#pragma once

#include <string>
#include <vector>

class HeartbeatRequest
{
	std::string sessionToken;
public:
	HeartbeatRequest(const std::string& sessionToken);
	std::vector<unsigned char> Serialize();
};