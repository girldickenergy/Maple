#pragma once

#include <vector>
#include <string.h>
#include <regex>
#include "Security/xorstr.hpp"

class StringUtilities
{
public:
	static std::vector<std::string> Split(const std::string str, const std::string rgx_str = xor("(0xdeadbeef)"))
	{
		const std::regex re(rgx_str);
		const std::sregex_token_iterator it{
			str.begin(),
			str.end(), re, -1
		};
		std::vector<std::string> tokenized{ it, {} };

		tokenized.erase(
			std::remove_if(tokenized.begin(),
				tokenized.end(),
				[](const std::string & s)
				{
					return s.size() == 0;
				}),
			tokenized.end());

		return tokenized;
	}

	static std::vector<std::string> Split(std::string input, char delimiter)
	{
		std::vector<std::string> ret = std::vector<std::string>();
		int init_size = strlen(input.c_str());

		char* ptr = strtok(_strdup(input.c_str()), &delimiter);

		while (ptr != nullptr)
		{
			// Copy the element, instead of pushing the reference.
			// pushing the reference can lead to undefined behavior, if destruction happens before construction
			// bug #0001
			ret.emplace_back(ptr);
			ptr = strtok(nullptr, &delimiter);
		}
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
