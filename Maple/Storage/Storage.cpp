#include "Storage.h"

#include <windows.h>
#include <filesystem>
#include <fstream>

#include "VirtualizerSDK.h"

#include "StorageConfig.h"
#include "xorstr.hpp"
#include "../Utilities/Crypto/CryptoUtilities.h"

void Storage::Initialize(const std::string& uniqueName)
{
    char* val;
    size_t len;
    errno_t err = _dupenv_s(&val, &len, xorstr_("APPDATA"));

    std::string appdata(val);

    StorageDirectory = appdata += xorstr_("\\") + uniqueName;
    ConfigsDirectory = StorageDirectory + xorstr_("\\configs");
    ProfilesDirectory = StorageDirectory + xorstr_("\\profiles");
    LogsDirectory = StorageDirectory + xorstr_("\\logs");

    storageConfigFilepath = StorageDirectory + xorstr_("\\") + uniqueName + xorstr_(".cfg");

    EnsureDirectoryExists(StorageDirectory);

    if (std::filesystem::exists(storageConfigFilepath))
    {
        std::ifstream storageConfig(storageConfigFilepath, std::ifstream::binary);
        Config.Deserialize(storageConfig);
        storageConfig.close();
    }
}

void Storage::EnsureDirectoryExists(const std::string& directory)
{
    if (!std::filesystem::exists(directory))
        std::filesystem::create_directory(directory);
}

void Storage::SaveStorageConfig()
{
    EnsureDirectoryExists(StorageDirectory);

    std::ofstream storageConfig(storageConfigFilepath, std::ofstream::out | std::ofstream::binary | std::ofstream::trunc);
    Config.Serialize(storageConfig);
    storageConfig.close();
}