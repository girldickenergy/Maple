#pragma once 

#include <string> 
#include <vector> 

class AuthStreamStageOneRequest
{
	std::string checksum;
public:
	AuthStreamStageOneRequest(const std::string& checksum);
	std::vector<unsigned char> Serialize();
};