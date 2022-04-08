#include "Storage.h"

#include "../Utilities/Security/xorstr.hpp"
#include "StorageConfig.h"
#include <ThemidaSDK.h>

#include <filesystem>
#include <fstream>

bool Storage::isSameName(const std::string& a, const std::string& b)
{
    std::string aLowerCase = a;
    std::string bLowerCase = b;

    std::transform(aLowerCase.begin(), aLowerCase.end(), aLowerCase.begin(), tolower);
    std::transform(bLowerCase.begin(), bLowerCase.end(), bLowerCase.begin(), tolower);

    return aLowerCase == bLowerCase;
}

void Storage::loadStorageConfig()
{
    StorageConfig::DefaultConfig = "default";
    StorageConfig::DefaultProfile = "none";

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
            if (variable == "DefaultConfig")
                StorageConfig::DefaultConfig = value;
            else if (variable == "DefaultProfile")
                StorageConfig::DefaultProfile = value;
        }
    }

    configFile.close();
}

void Storage::Initialize(const std::string& uniqueName)
{
    VM_FISH_RED_START
    STR_ENCRYPT_START

    char* val;
    size_t len;
    errno_t err = _dupenv_s(&val, &len, xor ("APPDATA"));

    std::string appdata(val);

    StorageDirectory = appdata += "\\" + uniqueName;
    ConfigsDirectory = StorageDirectory + "\\configs";
    ProfilesDirectory = StorageDirectory + "\\profiles";
    LogsDirectory = StorageDirectory + "\\logs";

    storageConfigFilepath = StorageDirectory + "\\" + uniqueName + ".cfg";

    loadStorageConfig();

    VM_FISH_RED_END
    STR_ENCRYPT_END
}

bool Storage::IsValidFileName(const std::string& filename)
{
    return !filename.empty() && !isSameName(filename, "default") && !isSameName(filename, "none");
}

void Storage::EnsureDirectoryExists(const std::string& directory)
{
    if (!std::filesystem::exists(directory))
        std::filesystem::create_directory(directory);
}

void Storage::SaveStorageConfig()
{
    EnsureDirectoryExists(StorageDirectory);

    std::ofstream ofs;
    ofs.open(storageConfigFilepath, std::ofstream::out | std::ofstream::trunc);

    ofs << "DefaultConfig=" << StorageConfig::DefaultConfig << std::endl;
    ofs << "DefaultProfile=" << StorageConfig::DefaultProfile << std::endl;

    ofs.close();
}
