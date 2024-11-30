#pragma once

#include "../IPacket.h"

enum class HeartbeatResult : uint8_t
{
	Success = 0,
	InvalidSession = 1,
	UnknownError = 2
};

class HeartbeatResponse : public IPacket
{
	HeartbeatResult m_Result;

	HeartbeatResponse();
public:
	HeartbeatResult GetResult();

	uint32_t GetIdentifier() override;
};
