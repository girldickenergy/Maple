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
			configName.Deserialize(CryptoUtilities::Base32Decode(file.path().filename().stem().string()));

			if (configName != xorstr_("default"))
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
			configFileName.Deserialize(CryptoUtilities::Base32Decode(file.path().filename().stem().string()));

			if (configFileName == configName)
                return file.path().string();
		}
	}

	return {};
}

std::string ConfigManager::getConfigPathForName(const EncryptedString& configName)
{
    Storage::EnsureDirectoryExists(Storage::ConfigsDirectory);

	return Storage::ConfigsDirectory + xorstr_("\\") + CryptoUtilities::Base32Encode(configName.Serialize()) + xorstr_(".cfg");
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

    CurrentConfigIndex = getConfigIndexByName(Storage::Config.DefaultConfig);

	Load();
}

void ConfigManager::Load()
{
	Storage::EnsureDirectoryExists(Storage::ConfigsDirectory);

	Storage::Config.DefaultConfig = Configs[CurrentConfigIndex];
	Storage::SaveStorageConfig();

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

	const std::string decodedConfigData = CryptoUtilities::Base64Decode(encodedConfigData);
	const std::vector<std::string> decodedConfigDataSplit = StringUtilities::Split(decodedConfigData, xorstr_("|||"));

	if (decodedConfigDataSplit.size() < 2 || decodedConfigDataSplit.size() > 2)
		return;

	EncryptedString configName;
	configName.Deserialize(CryptoUtilities::Base32Decode(decodedConfigDataSplit[0]));

	if (configName == xorstr_("default"))
		return;

	const std::vector<uint8_t> configData = CryptoUtilities::Base64DecodeToBytes(decodedConfigDataSplit[1]);

	if (!getConfigPathByName(configName).empty())
	{
        int i = 1;
		while (!getConfigPathByName(configName + xorstr_("(") + std::to_string(i).c_str() + xorstr_(")")).empty())
			i++;

		// todo: fix += operator?
		configName = configName + xorstr_("(") + std::to_string(i).c_str() + xorstr_(")");
	}

	std::ofstream configFile(getConfigPathForName(configName), std::ofstream::out | std::ofstream::binary | std::ofstream::trunc);
	configFile.write(reinterpret_cast<const char*>(configData.data()), configData.size());
	configFile.close();

	refresh();

    CurrentConfigIndex = getConfigIndexByName(configName);

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

	ClipboardUtilities::Write(CryptoUtilities::Base64Encode(CryptoUtilities::Base32Encode(Configs[CurrentConfigIndex].Serialize()) + xorstr_("|||") + CryptoUtilities::Base64Encode(configData)));
}

void ConfigManager::Rename()
{
	Storage::EnsureDirectoryExists(Storage::ConfigsDirectory);
	
	if (CurrentConfigIndex == 0)
		return;

	if (RenamedConfigName == xorstr_("default") || RenamedConfigName == Configs[CurrentConfigIndex])
        return;

	EncryptedString renamedConfigName = RenamedConfigName;

	if (!getConfigPathByName(renamedConfigName).empty())
	{
        int i = 1;
		while (!getConfigPathByName(renamedConfigName + xorstr_("(") + std::to_string(i).c_str() + xorstr_(")")).empty())
			i++;

		// todo: fix += operator?
		renamedConfigName = renamedConfigName + xorstr_("(") + std::to_string(i).c_str() + xorstr_(")");
	}

	std::rename(getConfigPathByName(Configs[CurrentConfigIndex]).c_str(), getConfigPathForName(renamedConfigName).c_str());

	refresh();

	CurrentConfigIndex = getConfigIndexByName(renamedConfigName);
}

void ConfigManager::Create()
{
	Storage::EnsureDirectoryExists(Storage::ConfigsDirectory);

	if (NewConfigName == xorstr_("default"))
		return;

	CurrentConfig = {};

	EncryptedString newConfigName = NewConfigName;

	if (!getConfigPathByName(newConfigName).empty())
	{
        int i = 1;
		while (!getConfigPathByName(newConfigName + xorstr_("(") + std::to_string(i).c_str() + xorstr_(")")).empty())
            i++;

		// todo: fix += operator?
		newConfigName = newConfigName + xorstr_("(") + std::to_string(i).c_str() + xorstr_(")");
	}

	std::ofstream configFile(getConfigPathForName(newConfigName), std::ofstream::out | std::ofstream::trunc | std::ofstream::binary);
    CurrentConfig.Serialize(configFile);
	configFile.close();

	refresh();

	CurrentConfigIndex = getConfigIndexByName(newConfigName);
}