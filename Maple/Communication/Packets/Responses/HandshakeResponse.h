#pragma once

#include <vector>
#include "../IPacket.h"

class HandshakeResponse : public IPacket
{
	std::vector<uint8_t> m_Key;
	std::vector<uint8_t> m_EncryptedKey;
	std::vector<uint8_t> m_EncryptedIv;

	HandshakeResponse();
public:
	const std::vector<uint8_t>& GetKey();
	const std::vector<uint8_t>& GetEncryptedKey();
	const std::vector<uint8_t>& GetEncryptedIv();

	uint32_t GetIdentifier() override;
};
