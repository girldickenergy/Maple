#include "AnticheatUtilities.h"

#include <windows.h>

#include "files.h"
#include "hex.h"
#include "sha.h"
#include "ThemidaSDK.h"

#include "../Security/xorstr.hpp"

std::string AnticheatUtilities::getAnticheatChecksum()
{
    STR_ENCRYPT_START
	
    char buffer[MAX_PATH];
    GetModuleFileNameA(NULL, buffer, MAX_PATH);
    const std::string::size_type pos = std::string(buffer).find_last_of(xorstr_("\\/"));

    const std::string osuAuthFilePath = std::string(buffer).substr(0, pos) + xorstr_("\\osu!auth.dll");

    std::string checksum;
    CryptoPP::SHA256 algo;
    CryptoPP::FileSource fs(osuAuthFilePath.c_str(), true, new CryptoPP::HashFilter(algo, new CryptoPP::HexEncoder(new CryptoPP::StringSink(checksum))));

    STR_ENCRYPT_END

    return checksum;
}

bool AnticheatUtilities::IsRunningGoodKnownVersion()
{
    return std::ranges::find(goodKnownChecksums, getAnticheatChecksum()) != goodKnownChecksums.end();
}
