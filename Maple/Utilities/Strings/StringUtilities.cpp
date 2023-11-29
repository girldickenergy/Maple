#include "StringUtilities.h"

#include <random>

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

std::string StringUtilities::GenerateRandomString(size_t size)
{
    static const char charset[] = 
		"0123456789"
		"abcdefghijklmnopqrstuvwxyz";

	std::random_device rd;
	std::mt19937 generator(rd());
    std::uniform_int_distribution distribution(0, static_cast<int>(strlen(charset)) - 1);

    std::string str;
    for (int i = 0; i < size; ++i)
        str += charset[distribution(generator)];

    return str;
}

std::string StringUtilities::GenerateRandomString(size_t size, const std::string& seed)
{
    constexpr std::hash<std::string> hasher{};

    const size_t seedHashed = hasher(seed);

	if (randomStringLookupTable.contains(seedHashed))
		return randomStringLookupTable[seedHashed];

    static const char charset[] = 
		"0123456789"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz";

	std::default_random_engine generator(seedHashed);
    std::uniform_int_distribution distribution(0, static_cast<int>(strlen(charset)) - 1);

    std::string str;
    for (int i = 0; i < size; ++i)
        str += charset[distribution(generator)];

	randomStringLookupTable[seedHashed] = str;

    return str;
}
