#pragma once

#include <vector>
#include <string>
#include <regex>
#include <unordered_map>

class StringUtilities
{
	static inline std::unordered_map<size_t, std::string> randomStringLookupTable = {};
public:
	static std::vector<std::string> Split(const std::string& s, const std::string& delimiter = "0xdeadbeef");
	static std::vector<unsigned char> StringToByteArray(const std::string str);
	static std::string ByteArrayToString(const std::vector<unsigned char> arr);
    static std::string GenerateRandomString(size_t size);
    static std::string GenerateRandomString(size_t size, const std::string& seed);
};
