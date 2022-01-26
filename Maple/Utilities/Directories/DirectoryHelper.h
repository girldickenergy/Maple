#pragma once

#include <ThemidaSDK.h>
#include <string>
#include <filesystem>

#include "../Crypto/CryptoHelper.h"
#include "../Security/xorstr.hpp"
#include "../../Communication/Communication.h"

class DirectoryHelper
{
public:
    static inline std::string WorkingDirectory;
    static inline std::string ConfigsDirectory;
    static inline std::string ProfilesDirectory;
    static inline std::string LogsDirectory;

    static void Initialize()
    {
        VM_FISH_RED_START
        STR_ENCRYPT_START

        char* val;
        size_t len;
        errno_t err = _dupenv_s(&val, &len, xor ("APPDATA"));

        std::string appdata(val);

        WorkingDirectory = appdata += "\\" + Communication::CurrentUser->UsernameHashed;
        ConfigsDirectory = WorkingDirectory + "\\configs";
        ProfilesDirectory = WorkingDirectory + "\\profiles";
        LogsDirectory = WorkingDirectory + "\\logs";

        VM_FISH_RED_END
        STR_ENCRYPT_END
    }

    static void EnsureDirectoriesExist()
    {
        if (!std::filesystem::exists(WorkingDirectory))
            std::filesystem::create_directory(WorkingDirectory);

        if (!std::filesystem::exists(ConfigsDirectory))
            std::filesystem::create_directory(ConfigsDirectory);

        if (!std::filesystem::exists(ProfilesDirectory))
            std::filesystem::create_directory(ProfilesDirectory);

        if (!std::filesystem::exists(LogsDirectory))
            std::filesystem::create_directory(LogsDirectory);
    }
};