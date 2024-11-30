#pragma once

#include "../IPacket.h"

class PingResponse : public IPacket
{
	PingResponse();
public:
	uint32_t GetIdentifier() override;
};
