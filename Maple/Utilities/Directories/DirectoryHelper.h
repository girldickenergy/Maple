#pragma once

#include <ThemidaSDK.h>
#include <string>
#include <filesystem>
#include <fstream>

#include "../Crypto/CryptoHelper.h"
#include "../Security/xorstr.hpp"
#include "../../Communication/Communication.h"

class DirectoryHelper
{
    static inline std::string configFilePath;

    static void loadConfig()
    {
        DirectoryHelper::EnsureDirectoriesExist();

        if (!std::filesystem::exists(configFilePath))
            return;

        std::ifstream file(configFilePath);
        std::string line;

        while (std::getline(file, line))
        {
            const int delimiterIndex = line.find('=');
            std::string variable = line.substr(0, delimiterIndex);
            std::string value = line.substr(delimiterIndex + 1, std::string::npos);

            if (!value.empty())
            {
                if (variable == "DefaultConfig")
                    DefaultConfig = value;
                else if (variable == "DefaultProfile")
                    DefaultProfile = value;
            }
        }

        file.close();
    }
public:
    static inline std::string WorkingDirectory;
    static inline std::string ConfigsDirectory;
    static inline std::string ProfilesDirectory;
    static inline std::string LogsDirectory;

    static inline std::string DefaultConfig = "default";
    static inline std::string DefaultProfile = "none";

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

        configFilePath = DirectoryHelper::WorkingDirectory + "\\" + Communication::CurrentUser->UsernameHashed + ".cfg";

        loadConfig();

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

    static void SaveConfig()
    {
        EnsureDirectoriesExist();

        std::ofstream ofs;
        ofs.open(configFilePath, std::ofstream::out | std::ofstream::trunc);

        ofs << "DefaultConfig=" << DefaultConfig << std::endl;
        ofs << "DefaultProfile=" << DefaultProfile << std::endl;

        ofs.close();
    }
};