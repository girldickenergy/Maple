#pragma once

#include "../IPacket.h"

class HandshakeRequest : public IPacket
{
	std::vector<std::uint32_t> m_RandomJunk;
public:
	HandshakeRequest(const std::vector<std::uint32_t>& randomJunk);

	uint32_t GetIdentifier() override;
};
