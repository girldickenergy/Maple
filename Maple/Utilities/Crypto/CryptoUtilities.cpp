#include "CryptoUtilities.h"

#include <windows.h>

#define CRYPTOPP_ENABLE_NAMESPACE_WEAK 1
#include "base64.h"
#include "md5.h"
#include "hex.h"

std::string CryptoUtilities::GetMD5Hash(const std::string& str)
{
    int size = MultiByteToWideChar(CP_ACP, MB_COMPOSITE, str.c_str(), str.length(), nullptr, 0);

    std::wstring utf16_str(size, '\0');
    MultiByteToWideChar(CP_ACP, MB_COMPOSITE, str.c_str(), str.length(), &utf16_str[0], size);

    int utf8_size = WideCharToMultiByte(CP_UTF8, 0, utf16_str.c_str(), utf16_str.length(), nullptr, 0, nullptr, nullptr);
    std::string utf8_str(utf8_size, '\0');
    WideCharToMultiByte(CP_UTF8, 0, utf16_str.c_str(), utf16_str.length(), &utf8_str[0], utf8_size, nullptr, nullptr);

    CryptoPP::Weak1::MD5 hash;
    byte digest[CryptoPP::Weak1::MD5::DIGESTSIZE];

    hash.CalculateDigest(digest, (byte*)utf8_str.c_str(), utf8_str.length());

    CryptoPP::HexEncoder encoder;
    std::string strHashed;
    encoder.Attach(new CryptoPP::StringSink(strHashed));
    encoder.Put(digest, sizeof(digest));
    encoder.MessageEnd();

    std::transform(strHashed.begin(), strHashed.end(), strHashed.begin(), ::tolower);

    return strHashed;
}

std::wstring CryptoUtilities::GetMD5Hash(const std::wstring& str)
{
    int utf8_size = WideCharToMultiByte(CP_UTF8, 0, str.c_str(), str.length(), nullptr, 0, nullptr, nullptr);
    std::string utf8_str(utf8_size, '\0');
    WideCharToMultiByte(CP_UTF8, 0, str.c_str(), str.length(), &utf8_str[0], utf8_size, nullptr, nullptr);

    CryptoPP::Weak1::MD5 hash;
    byte digest[CryptoPP::Weak1::MD5::DIGESTSIZE];

    hash.CalculateDigest(digest, (byte*)utf8_str.c_str(), utf8_str.length());

    CryptoPP::HexEncoder encoder;
    std::string strHashed;
    encoder.Attach(new CryptoPP::StringSink(strHashed));
    encoder.Put(digest, sizeof(digest));
    encoder.MessageEnd();

    std::transform(strHashed.begin(), strHashed.end(), strHashed.begin(), ::tolower);

    return std::wstring(strHashed.begin(), strHashed.end());
}

std::string CryptoUtilities::MapleXOR(const std::string& str, const std::string& key)
{
    std::string result = str;

    unsigned int j = 0;
    for (unsigned int i = 0; i < str.length(); i++)
    {
        result[i] = str[i] ^ key[j];

        j = (++j < key.length() ? j : 0);
    }

    return result;
}

std::string CryptoUtilities::Base64Encode(const std::string& str)
{
    std::string result;

    CryptoPP::Base64Encoder encoder(nullptr, false);
    encoder.Attach(new CryptoPP::StringSink(result));
    encoder.Put(reinterpret_cast<const uint8_t*>(str.data()), str.length());
    encoder.MessageEnd();

    return result;
}

std::string CryptoUtilities::Base64Decode(const std::string& str)
{
    std::string result;

    CryptoPP::Base64Decoder decoder;
    decoder.Attach(new CryptoPP::StringSink(result));
    decoder.Put(reinterpret_cast<const uint8_t*>(str.data()), str.length());
    decoder.MessageEnd();

    return result;
}
