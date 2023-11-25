#pragma once

#include <string>
#include <vector>

class CryptoUtilities
{
public:
    static std::string GetMD5Hash(const std::string& str);
    static std::wstring GetMD5Hash(const std::wstring& str);
    static std::string MapleXOR(const std::string& str, const std::string& key);
    static std::vector<uint8_t> MapleXOR(const std::vector<uint8_t>& data, const std::string& key);
    static std::string Base64Encode(const std::string& str);
    static std::string Base64Encode(const std::vector<uint8_t>& data);
    static std::string Base64Decode(const std::string& str);
    static std::vector<uint8_t> Base64DecodeToBytes(const std::string &str);
};
