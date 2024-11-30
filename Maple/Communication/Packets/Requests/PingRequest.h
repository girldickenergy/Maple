#pragma once

#include <string>
#include "../IPacket.h"

class PingRequest : public IPacket
{
public:
	PingRequest();

	uint32_t GetIdentifier() override;
};