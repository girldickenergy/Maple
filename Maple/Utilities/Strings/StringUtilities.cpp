#include "StringUtilities.h"

std::vector<std::string> StringUtilities::Split(const std::string& s, const std::string& delimiter)
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

std::vector<unsigned char> StringUtilities::StringToByteArray(const std::string str)
{
	std::vector<unsigned char> arr;
	for (const auto& c : str)
		arr.push_back(c);

	return arr;
}

std::string StringUtilities::ByteArrayToString(const std::vector<unsigned char> arr)
{
	std::string str;
	for (const auto& c : arr)
		str.push_back(c);

	return str;
}
