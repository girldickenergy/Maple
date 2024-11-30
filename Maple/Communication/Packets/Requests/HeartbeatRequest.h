#pragma once

#include <string>
#include "../IPacket.h"

class HeartbeatRequest : public IPacket
{
	std::string m_SessionToken;
public:
	HeartbeatRequest(const std::string& sessionToken);

	uint32_t GetIdentifier() override;
};