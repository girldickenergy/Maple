#pragma once

#include <string>

class CryptoUtilities
{
public:
    static std::string GetMD5Hash(const std::string& str);
    static std::wstring GetMD5Hash(const std::wstring& str);
};