#pragma once 

#include <string>
#include <vector>
#include "../IPacket.h"

class AuthStreamStageTwoRequest : public IPacket
{
	std::string m_Username;
	std::string m_Checksum;
	std::vector<uint8_t> m_AuthBytes;
	std::vector<uint8_t> m_OsuBytes;
public:
	AuthStreamStageTwoRequest(const std::string& username, const std::string& checksum, const std::vector<uint8_t>& authBytes, const std::vector<uint8_t>& osuBytes);

	uint32_t GetIdentifier() override;
};