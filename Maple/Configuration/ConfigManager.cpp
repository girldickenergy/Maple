#include "ConfigManager.h"

#include <filesystem>
#include <fstream>

#include "xorstr.hpp"

#include "../Storage/Storage.h"
#include "../Storage/StorageConfig.h"
#include "../Utilities/Clipboard/ClipboardUtilities.h"
#include "../Utilities/Crypto/CryptoUtilities.h"
#include "../Utilities/Strings/StringUtilities.h"

void ConfigManager::refresh()
{
	Storage::EnsureDirectoryExists(Storage::ConfigsDirectory);

	Configs.clear();
	Configs.emplace_back(xorstr_("default"));

	for (const auto& file : std::filesystem::directory_iterator(Storage::ConfigsDirectory))
	{
		if (file.path().extension() == xorstr_(".cfg"))
		{
            EncryptedString configName;

            std::ifstream configFile(file.path(), std::ifstream::binary);

            configFile.seekg(0, std::ios::end);
            if (const int length = configFile.tellg(); length < sizeof(float) + sizeof(char) + sizeof(size_t))
				continue;
            
			configFile.seekg(sizeof(float), std::ifstream::beg);
            configName.Deserialize(configFile);
            configFile.close();

			Configs.push_back(configName);
		}
	}

	std::sort(Configs.begin() + 1, Configs.end(), [](const EncryptedString& a, const EncryptedString& b) -> bool
    {
        return a < b;
    });
}

std::string ConfigManager::getConfigPathByName(const EncryptedString& configName)
{
	Storage::EnsureDirectoryExists(Storage::ConfigsDirectory);

	for (const auto& file : std::filesystem::directory_iterator(Storage::ConfigsDirectory))
	{
		if (file.path().extension() == xorstr_(".cfg"))
		{
            EncryptedString configFileName;

            std::ifstream configFile(file.path(), std::ifstream::binary);

            configFile.seekg(0, std::ios::end);
            if (const int length = configFile.tellg(); length < sizeof(float) + sizeof(char) + sizeof(size_t))
				continue;

			configFile.seekg(sizeof(float), std::ifstream::beg);
            configFileName.Deserialize(configFile);
            configFile.close();

			if (configFileName == configName)
                return file.path().string();
		}
	}

	return {};
}

std::string ConfigManager::getUniqueConfigPath()
{
    Storage::EnsureDirectoryExists(Storage::ConfigsDirectory);

	std::string configFilePath;

	do
	{
		configFilePath = Storage::ConfigsDirectory + xorstr_("\\") + StringUtilities::GenerateRandomString(16) + xorstr_(".cfg");
	} while (std::filesystem::exists(configFilePath));

	return configFilePath;
}

int ConfigManager::getConfigIndexByName(const EncryptedString& configName)
{
    for (int i = 0; i < Configs.size(); i++)
        if (Configs[i] == configName)
                return i;

	return 0;
}

void ConfigManager::Initialize()
{
	CurrentConfigIndex = 0;

	refresh();

    CurrentConfigIndex = getConfigIndexByName(StorageConfig::DefaultConfig.c_str());

	Load();
}

void ConfigManager::Load()
{
	Storage::EnsureDirectoryExists(Storage::ConfigsDirectory);

	//StorageConfig::DefaultConfig = Configs[CurrentConfigIndex];
	//Storage::SaveStorageConfig();

	if (CurrentConfigIndex == 0)
	{
		CurrentConfig = {};

        return;
	}

	const std::string configFilePath = getConfigPathByName(Configs[CurrentConfigIndex]);
    if (configFilePath.empty() || !std::filesystem::exists(configFilePath))
    {
        refresh();
        CurrentConfigIndex = 0;
        CurrentConfig = {};

        return;
	}

	std::ifstream configFile(configFilePath, std::ifstream::binary);
    CurrentConfig.Deserialize(configFile);
    configFile.close();
}

void ConfigManager::Save()
{
	if (CurrentConfigIndex == 0)
		return;

	Storage::EnsureDirectoryExists(Storage::ConfigsDirectory);

	std::ofstream configFile(getConfigPathByName(Configs[CurrentConfigIndex]), std::ofstream::out | std::ofstream::trunc | std::ofstream::binary);
    CurrentConfig.Serialize(configFile);
	configFile.close();
}

void ConfigManager::Delete()
{
	Storage::EnsureDirectoryExists(Storage::ConfigsDirectory);

	if (CurrentConfigIndex == 0)
		return;

	const std::string configFilePath = getConfigPathByName(Configs[CurrentConfigIndex]);
    if (!configFilePath.empty())
        std::filesystem::remove(configFilePath);
	
	refresh();

	CurrentConfigIndex = 0;

	Load();
}

void ConfigManager::Import()
{
	Storage::EnsureDirectoryExists(Storage::ConfigsDirectory);

	const std::string encodedConfigData = ClipboardUtilities::Read();

	if (encodedConfigData.empty())
		return;

	const std::vector<uint8_t> decodedConfigData = CryptoUtilities::Base64DecodeToBytes(encodedConfigData);

	const std::string configFilePath = getUniqueConfigPath();

	std::ofstream configFile(configFilePath, std::ofstream::out | std::ofstream::binary | std::ofstream::trunc);
	configFile.write(reinterpret_cast<const char*>(decodedConfigData.data()), decodedConfigData.size());
	configFile.close();

	std::ifstream configFile2(configFilePath, std::ifstream::binary);
    Config tempConfig;
	tempConfig.Deserialize(configFile2);
    configFile2.close();

	if (!getConfigPathByName(tempConfig.Name).empty())
	{
        int i = 1;
		while (!getConfigPathByName(tempConfig.Name + xorstr_("(") + std::to_string(i).c_str() + xorstr_(")")).empty())
			i++;

		// todo: fix += operator?
		tempConfig.Name = tempConfig.Name + xorstr_("(") + std::to_string(i).c_str() + xorstr_(")");

		std::ofstream configFile3(configFilePath, std::ofstream::out | std::ofstream::binary | std::ofstream::trunc);
        tempConfig.Serialize(configFile3);
		configFile3.close();
	}

	refresh();

    CurrentConfigIndex = getConfigIndexByName(tempConfig.Name);

	Load();
}

void ConfigManager::Export()
{
	Storage::EnsureDirectoryExists(Storage::ConfigsDirectory);

	if (CurrentConfigIndex == 0)
		return;

	const std::string configFilePath = getConfigPathByName(Configs[CurrentConfigIndex]);

	std::ifstream configFile(configFilePath, std::ifstream::binary);
	const std::vector<uint8_t> configData((std::istreambuf_iterator<char>(configFile)), std::istreambuf_iterator<char>());
	configFile.close();

	ClipboardUtilities::Write(CryptoUtilities::Base64Encode(configData));
}

void ConfigManager::Rename()
{
	Storage::EnsureDirectoryExists(Storage::ConfigsDirectory);
	
	if (CurrentConfigIndex == 0)
		return;

	if (RenamedConfigName == xorstr_("default"))
        return;

	if (!getConfigPathByName(RenamedConfigName).empty())
	{
        int i = 1;
		while (!getConfigPathByName(RenamedConfigName + xorstr_("(") + std::to_string(i).c_str() + xorstr_(")")).empty())
			i++;

		// todo: fix += operator?
		CurrentConfig.Name = RenamedConfigName + xorstr_("(") + std::to_string(i).c_str() + xorstr_(")");
	}
	else
        CurrentConfig.Name = RenamedConfigName;

	Save();

	refresh();

	CurrentConfigIndex = getConfigIndexByName(CurrentConfig.Name);
}

void ConfigManager::Create()
{
	Storage::EnsureDirectoryExists(Storage::ConfigsDirectory);

	if (NewConfigName == xorstr_("default"))
		return;

	CurrentConfig = {};
    CurrentConfig.Name = NewConfigName;
	if (!getConfigPathByName(NewConfigName).empty())
	{
        int i = 1;
		while (!getConfigPathByName(NewConfigName + xorstr_("(") + std::to_string(i).c_str() + xorstr_(")")).empty())
		{
            i++;
		}

		// todo: fix += operator?
		CurrentConfig.Name = NewConfigName + xorstr_("(") + std::to_string(i).c_str() + xorstr_(")");
	}

	std::ofstream configFile(getUniqueConfigPath(), std::ofstream::out | std::ofstream::trunc | std::ofstream::binary);
    CurrentConfig.Serialize(configFile);
	configFile.close();

	refresh();

	CurrentConfigIndex = getConfigIndexByName(CurrentConfig.Name);
}