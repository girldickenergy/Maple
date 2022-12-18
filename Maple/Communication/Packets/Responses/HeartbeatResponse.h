#pragma once

#include <vector>

enum class HeartbeatResult
{
	Success = 0,
	InvalidSession = 1,
	UnknownError = 2
};

class HeartbeatResponse
{
	HeartbeatResult result;

	HeartbeatResponse(HeartbeatResult result);
public:
	HeartbeatResult GetResult();

	static HeartbeatResponse Deserialize(const std::vector<unsigned char>& payload);
};
