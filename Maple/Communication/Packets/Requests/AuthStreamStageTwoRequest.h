#pragma once 

#include <string> 
#include <vector> 

class AuthStreamStageTwoRequest
{
	std::string username;
	std::string checksum;
	std::vector<uint8_t> authBytes;
	std::vector<uint8_t> osuBytes;
public:
	AuthStreamStageTwoRequest(const std::string& username, const std::string& checksum, const std::vector<uint8_t>& authBytes, const std::vector<uint8_t>& osuBytes);
	std::vector<unsigned char> Serialize();
};