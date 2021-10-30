#pragma once

#include <vector>
#include <string>
#include <regex>

class StringUtilities
{
public:
	static std::vector<std::string> Split(const std::string& s, const std::string& delimiter = "0xdeadbeef")
	{
		std::string str = s;
		size_t pos = 0;
		std::string token;
		std::vector<std::string> ret;
		while ((pos = str.find(delimiter)) != std::string::npos) {
			token = str.substr(0, pos);
			ret.push_back(token);
			str.erase(0, pos + delimiter.length());
		}
		ret.push_back(str);

		return ret;
	}

	static std::vector<unsigned char> StringToByteArray(const std::string str)
	{
		std::vector<unsigned char> arr;
		for (const auto& c : str)
			arr.push_back(c);

		return arr;
	}

	static std::string ByteArrayToString(const std::vector<unsigned char> arr)
	{
		std::string str;
		for (const auto& c : arr)
			str.push_back(c);

		return str;
	}
};
