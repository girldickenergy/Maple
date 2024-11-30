#pragma once 

#include <string>
#include "../IPacket.h"

class AuthStreamStageOneRequest : public IPacket
{
	std::string m_Checksum;
public:
	AuthStreamStageOneRequest(const std::string& checksum);

	uint32_t GetIdentifier() override;
};