#pragma once

#include <vector>
#include "../IPacket.h"

class HandshakeResponse : public IPacket
{
	std::vector<uint8_t> m_Key;
	std::vector<uint8_t> m_Iv;

	HandshakeResponse();
public:
	const std::vector<uint8_t>& GetKey();
	const std::vector<uint8_t>& GetIV();

	uint32_t GetIdentifier() override;
};
