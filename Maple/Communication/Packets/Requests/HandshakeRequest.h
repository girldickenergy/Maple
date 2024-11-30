#pragma once

#include "../IPacket.h"

class HandshakeRequest : public IPacket
{
	std::time_t m_Epoch;
public:
	HandshakeRequest();

	uint32_t GetIdentifier() override;
};
