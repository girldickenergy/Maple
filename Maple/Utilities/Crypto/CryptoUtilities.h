#pragma once

#include <string>

class CryptoUtilities
{
public:
    static std::string GetMD5Hash(const std::string& str);
    static std::wstring GetMD5Hash(const std::wstring& str);
    static void MapleXOR(std::string &str, const std::string &key);
    static std::string Base64Encode(const std::string& str);
    static std::string Base64Decode(const std::string& str);
};