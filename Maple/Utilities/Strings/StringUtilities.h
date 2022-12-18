#pragma once

#include <vector>
#include <string>
#include <regex>

class StringUtilities
{
public:
	static std::vector<std::string> Split(const std::string& s, const std::string& delimiter = "0xdeadbeef");
	static std::vector<unsigned char> StringToByteArray(const std::string str);
	static std::string ByteArrayToString(const std::vector<unsigned char> arr);
};