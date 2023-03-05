#include "AnticheatUtilities.h"

#include <windows.h>

#include "files.h"
#include "hex.h"
#include "md5.h"
#include "ThemidaSDK.h"

#include "../Security/xorstr.hpp"

std::string AnticheatUtilities::GetAnticheatChecksum()
{
    STR_ENCRYPT_START
	
    char buffer[MAX_PATH];
    GetModuleFileNameA(NULL, buffer, MAX_PATH);
    const std::string::size_type pos = std::string(buffer).find_last_of(xorstr_("\\/"));

    const std::string osuAuthFilePath = std::string(buffer).substr(0, pos) + xorstr_("\\osu!auth.dll");
	
    std::string checksum;
    CryptoPP::MD5 algo;
    CryptoPP::FileSource fs(osuAuthFilePath.c_str(), true, new CryptoPP::HashFilter(algo, new CryptoPP::HexEncoder(new CryptoPP::StringSink(checksum))));

    std::transform(checksum.begin(), checksum.end(), checksum.begin(), ::tolower);

    STR_ENCRYPT_END

    return checksum;
}

std::vector<uint8_t> AnticheatUtilities::GetAnticheatBytes()
{
    STR_ENCRYPT_START

    char buffer[MAX_PATH];
    GetModuleFileNameA(NULL, buffer, MAX_PATH);
    const std::string::size_type pos = std::string(buffer).find_last_of(xorstr_("\\/"));

    const std::string osuAuthFilePath = std::string(buffer).substr(0, pos) + xorstr_("\\osu!auth.dll");

    std::vector<unsigned char> bytes;

    std::ifstream file(osuAuthFilePath, std::ios::binary);

    bytes.insert(bytes.begin(),
        std::istreambuf_iterator<char>(file),
        std::istreambuf_iterator<char>());

    file.close();

    STR_ENCRYPT_END

    return bytes;
}

std::vector<uint8_t> AnticheatUtilities::GetGameBytes()
{
    STR_ENCRYPT_START

    char buffer[MAX_PATH];
    GetModuleFileNameA(NULL, buffer, MAX_PATH);
    const std::string osuFilePath = std::string(buffer);

    std::vector<unsigned char> bytes;

    std::ifstream file(osuFilePath, std::ios::binary);

    bytes.insert(bytes.begin(),
        std::istreambuf_iterator<char>(file),
        std::istreambuf_iterator<char>());

    file.close();

    STR_ENCRYPT_END

    return bytes;
}

bool AnticheatUtilities::IsRunningGoodKnownVersion()
{
    return std::ranges::find(goodKnownChecksums, GetAnticheatChecksum()) != goodKnownChecksums.end();
}
