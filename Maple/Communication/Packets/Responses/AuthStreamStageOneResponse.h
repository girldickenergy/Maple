#pragma once 

#include <vector> 

class AuthStreamStageOneResponse
{
	bool shouldSend;

	AuthStreamStageOneResponse(bool shouldSend);
public:
	bool GetShouldSend();

	static AuthStreamStageOneResponse Deserialize(const std::vector<unsigned char>& payload);
};
