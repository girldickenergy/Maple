#pragma once

#include <string>
#include <vector>

#include <ThemidaSDK.h>

#include "RequestType.h"

class Request
{
public:
	RequestType Type;
	std::vector<unsigned char> Data; //type | data size | data

	Request(RequestType type);

	void AddDelimiter(std::vector<unsigned char>* dest);
	void AddByte(unsigned char byte, std::vector<unsigned char>* dest);
	void AddString(std::string str, std::vector<unsigned char>* dest);
	void AddString(std::vector<unsigned char> str, std::vector<unsigned char>* dest);
	void AddByte(unsigned char byte);
	void AddString(std::string str);
	void AddString(std::vector<unsigned char> str);
};
