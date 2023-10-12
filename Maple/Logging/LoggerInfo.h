#pragma once

#include <string>

struct LoggerInfo
{
    std::string MapleVersion;
    std::string OsuHash;
    std::string OsuAuthHash;
    std::string WindowsVersion;
    std::string CLRVersion;

    LoggerInfo() = default;

    LoggerInfo(const std::string& mapleVersion, const std::string& osuHash, const std::string& osuAuthHash, const std::string& windowsVersion, const std::string& clrVersion)
    {
        MapleVersion = mapleVersion;
        OsuHash = osuHash;
        OsuAuthHash = osuAuthHash;
        WindowsVersion = windowsVersion;
        CLRVersion = clrVersion;
    }
};
