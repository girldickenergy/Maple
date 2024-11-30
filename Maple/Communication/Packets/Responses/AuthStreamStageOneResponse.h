#pragma once 

#include "../IPacket.h"

class AuthStreamStageOneResponse : public IPacket
{
	bool m_ShouldSend;

	AuthStreamStageOneResponse();
public:
	bool GetShouldSend();

	uint32_t GetIdentifier() override;
};
