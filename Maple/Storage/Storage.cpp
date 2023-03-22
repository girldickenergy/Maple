#include "Storage.h"

#include <windows.h>
#include <filesystem>
#include <fstream>

#include "VirtualizerSDK.h"

#include "StorageConfig.h"
#include "xorstr.hpp"

void Storage::loadStorageConfig()
{
    		
    StorageConfig::DefaultConfig = xorstr_("default");
    StorageConfig::DefaultProfile = xorstr_("none");
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
            if (variable == xorstr_("DefaultConfig"))
                StorageConfig::DefaultConfig = value;
            else if (variable == xorstr_("DefaultProfile"))
                StorageConfig::DefaultProfile = value;
            else if (variable == xorstr_("ShowMenuAfterInjection"))
                StorageConfig::ShowMenuAfterInjection = value == "1";
            else if (variable == xorstr_("MenuKey"))
                StorageConfig::MenuKey = std::stoi(value);
        }
    }

    configFile.close();

    }

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

    loadStorageConfig();

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
    return !filename.empty() && !IsSameFileName(filename, xorstr_("default")) && !IsSameFileName(filename, xorstr_("none"));
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

    ofs << xorstr_("DefaultConfig=") << StorageConfig::DefaultConfig << std::endl;
    ofs << xorstr_("DefaultProfile=") << StorageConfig::DefaultProfile << std::endl;
    ofs << xorstr_("ShowMenuAfterInjection=") << StorageConfig::ShowMenuAfterInjection << std::endl;
    ofs << xorstr_("MenuKey=") << StorageConfig::MenuKey << std::endl;

    ofs.close();

    }
