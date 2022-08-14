#include "Storage.h"

#include <windows.h>
#include <filesystem>
#include <fstream>

#include "ThemidaSDK.h"

#include "../Utilities/Security/xorstr.hpp"
#include "StorageConfig.h"

void Storage::loadStorageConfig()
{
    STR_ENCRYPT_START
		
    StorageConfig::DefaultConfig = xor ("default");
    StorageConfig::DefaultProfile = xor ("none");
    StorageConfig::ShowMenuAfterInjection = true;
    StorageConfig::MenuKey = VK_DELETE;

    EnsureDirectoryExists(StorageDirectory);

    if (!std::filesystem::exists(storageConfigFilepath))
        return;

    std::ifstream configFile(storageConfigFilepath);
    std::string line;

    while (std::getline(configFile, line))
    {
        const int delimiterIndex = line.find('=');
        std::string variable = line.substr(0, delimiterIndex);
        std::string value = line.substr(delimiterIndex + 1, std::string::npos);

        if (!value.empty())
        {
            if (variable == xor ("DefaultConfig"))
                StorageConfig::DefaultConfig = value;
            else if (variable == xor ("DefaultProfile"))
                StorageConfig::DefaultProfile = value;
            else if (variable == xor ("ShowMenuAfterInjection"))
                StorageConfig::ShowMenuAfterInjection = value == "1";
            else if (variable == xor ("MenuKey"))
                StorageConfig::MenuKey = std::stoi(value);
        }
    }

    configFile.close();

    STR_ENCRYPT_END
}

void Storage::Initialize(const std::string& uniqueName)
{
    STR_ENCRYPT_START

    char* val;
    size_t len;
    errno_t err = _dupenv_s(&val, &len, xor ("APPDATA"));

    std::string appdata(val);

    StorageDirectory = appdata += xor ("\\") + uniqueName;
    ConfigsDirectory = StorageDirectory + xor ("\\configs");
    ProfilesDirectory = StorageDirectory + xor ("\\profiles");
    LogsDirectory = StorageDirectory + xor ("\\logs");

    storageConfigFilepath = StorageDirectory + xor ("\\") + uniqueName + xor (".cfg");

    loadStorageConfig();

    STR_ENCRYPT_END
}

bool Storage::IsSameFileName(const std::string& a, const std::string& b)
{
    std::string aLowerCase = a;
    std::string bLowerCase = b;

    std::transform(aLowerCase.begin(), aLowerCase.end(), aLowerCase.begin(), tolower);
    std::transform(bLowerCase.begin(), bLowerCase.end(), bLowerCase.begin(), tolower);

    return aLowerCase == bLowerCase;
}

bool Storage::IsValidFileName(const std::string& filename)
{
    return !filename.empty() && !IsSameFileName(filename, xor ("default")) && !IsSameFileName(filename, xor ("none"));
}

void Storage::EnsureDirectoryExists(const std::string& directory)
{
    if (!std::filesystem::exists(directory))
        std::filesystem::create_directory(directory);
}

void Storage::SaveStorageConfig()
{
    STR_ENCRYPT_START
		
    EnsureDirectoryExists(StorageDirectory);

    std::ofstream ofs;
    ofs.open(storageConfigFilepath, std::ofstream::out | std::ofstream::trunc);

    ofs << xor ("DefaultConfig=") << StorageConfig::DefaultConfig << std::endl;
    ofs << xor ("DefaultProfile=") << StorageConfig::DefaultProfile << std::endl;
    ofs << xor ("ShowMenuAfterInjection=") << StorageConfig::ShowMenuAfterInjection << std::endl;
    ofs << xor ("MenuKey=") << StorageConfig::MenuKey << std::endl;

    ofs.close();

    STR_ENCRYPT_END
}
