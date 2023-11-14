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
		if (file.path().extension() == xorstr_(".cfg") && Storage::IsValidFileName(file.path().filename().stem().string()))
			Configs.push_back(file.path().filename().stem().string());
}

void ConfigManager::Initialize()
{
	CurrentConfigIndex = 0;

	refresh();

	const auto it = std::find(Configs.begin(), Configs.end(), StorageConfig::DefaultConfig);

	if (it != Configs.end())
	{
		CurrentConfigIndex = std::distance(Configs.begin(), it);

		Load();
	}
}

void ConfigManager::Load()
{
	CurrentConfig = {};

	Storage::EnsureDirectoryExists(Storage::ConfigsDirectory);

	StorageConfig::DefaultConfig = Configs[CurrentConfigIndex];
	Storage::SaveStorageConfig();

	if (CurrentConfigIndex == 0)
		return;

	const std::string configFilePath = Storage::ConfigsDirectory + xorstr_("\\") + Configs[CurrentConfigIndex] + xorstr_(".cfg");

	if (!std::filesystem::exists(configFilePath))
		return;

	std::ifstream file(configFilePath, std::ifstream::binary);
	file.read(reinterpret_cast<char*>(&CurrentConfig), sizeof(Config));
	file.close();
}

void ConfigManager::Save()
{
	if (CurrentConfigIndex == 0)
		return;

	Storage::EnsureDirectoryExists(Storage::ConfigsDirectory);

	const std::string configFilePath = Storage::ConfigsDirectory + xorstr_("\\") + Configs[CurrentConfigIndex] + xorstr_(".cfg");

	std::ofstream file(configFilePath, std::ofstream::out | std::ofstream::trunc | std::ofstream::binary);
	file.write(reinterpret_cast<char*>(&CurrentConfig), sizeof(Config));
	file.close();
}

void ConfigManager::Delete()
{
	Storage::EnsureDirectoryExists(Storage::ConfigsDirectory);

	if (CurrentConfigIndex == 0)
		return;

	const std::string configFilePath = Storage::ConfigsDirectory + xorstr_("\\") + Configs[CurrentConfigIndex] + xorstr_(".cfg");

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

	std::string decodedConfigData = CryptoUtilities::MapleXOR(CryptoUtilities::Base64Decode(encodedConfigData), xorstr_("kelxFFMHsiGnONNa"));
	const std::vector<std::string> decodedConfigDataSplit = StringUtilities::Split(decodedConfigData, xorstr_("|"));

	if (decodedConfigDataSplit.size() < 2 || decodedConfigDataSplit.size() > 2)
		return;

	std::string configName = CryptoUtilities::Base64Decode(decodedConfigDataSplit[0]);
	const std::string configData = CryptoUtilities::Base64Decode(decodedConfigDataSplit[1]);

	std::string configFilePath = Storage::ConfigsDirectory + xorstr_("\\") + configName + xorstr_(".cfg");

	if (!Storage::IsValidFileName(configName))
		return;

	if (std::filesystem::exists(configFilePath))
	{
		unsigned int i = 2;
		while (true)
		{
			const std::string newConfigName = configName + xorstr_("_") + std::to_string(i);
			const std::string newConfigFilePath = Storage::ConfigsDirectory + xorstr_("\\") + newConfigName + xorstr_(".cfg");
			if (!std::filesystem::exists(newConfigFilePath))
			{
				configName = newConfigName;
				configFilePath = newConfigFilePath;

				break;
			}

			i++;
		}
	}

	std::ofstream file(configFilePath, std::ofstream::out | std::ofstream::binary | std::ofstream::trunc);
	file << configData << std::endl;
	file.close();

	refresh();

	const auto it = std::find(Configs.begin(), Configs.end(), configName);

	if (it != Configs.end())
		CurrentConfigIndex = std::distance(Configs.begin(), it);

	Load();
}

void ConfigManager::Export()
{
	Storage::EnsureDirectoryExists(Storage::ConfigsDirectory);

	if (CurrentConfigIndex == 0)
		return;

	const std::string configFilePath = Storage::ConfigsDirectory + xorstr_("\\") + Configs[CurrentConfigIndex] + xorstr_(".cfg");

	std::ifstream file(configFilePath, std::ifstream::binary);
	const std::vector<uint8_t> configData((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	file.close();

	std::string encoded = CryptoUtilities::Base64Encode(Configs[CurrentConfigIndex]) + xorstr_("|") + CryptoUtilities::Base64Encode(configData);
	encoded = CryptoUtilities::Base64Encode(CryptoUtilities::MapleXOR(encoded, xorstr_("kelxFFMHsiGnONNa")));

	ClipboardUtilities::Write(encoded);
}

void ConfigManager::Rename()
{
	Storage::EnsureDirectoryExists(Storage::ConfigsDirectory);
	
	if (CurrentConfigIndex == 0)
		return;

	const std::string configFilePath = Storage::ConfigsDirectory + xorstr_("\\") + Configs[CurrentConfigIndex] + xorstr_(".cfg");

	if (!Storage::IsValidFileName(RenamedConfigName) || Storage::IsSameFileName(RenamedConfigName, Configs[CurrentConfigIndex]))
		return;

	std::string renamedConfigName = RenamedConfigName;
	std::string renamedConfigFilePath = Storage::ConfigsDirectory + xorstr_("\\") + renamedConfigName + xorstr_(".cfg");

	if (std::filesystem::exists(renamedConfigFilePath))
	{
		unsigned int i = 2;
		while (true)
		{
			const std::string newConfigName = renamedConfigName + xorstr_("_") + std::to_string(i);
			const std::string newConfigFilePath = Storage::ConfigsDirectory + xorstr_("\\") + newConfigName + xorstr_(".cfg");
			if (!std::filesystem::exists(newConfigFilePath))
			{
				renamedConfigName = newConfigName;
				renamedConfigFilePath = newConfigFilePath;

				break;
			}

			i++;
		}
	}
	
	std::rename(configFilePath.c_str(), renamedConfigFilePath.c_str());

	refresh();

	const auto it = std::find(Configs.begin(), Configs.end(), renamedConfigName);

	if (it != Configs.end())
		CurrentConfigIndex = std::distance(Configs.begin(), it);
}

void ConfigManager::Create()
{
	Storage::EnsureDirectoryExists(Storage::ConfigsDirectory);

	char newConfigName[32];
	NewConfigName.DecryptTo(newConfigName);

	std::string configName = newConfigName;
	std::string configFilePath = Storage::ConfigsDirectory + xorstr_("\\") + newConfigName + xorstr_(".cfg");

	if (!Storage::IsValidFileName(configName))
		return;

	if (std::filesystem::exists(configFilePath))
	{
		unsigned int i = 2;
		while (true)
		{
			const std::string newConfigName = configName + xorstr_("_") + std::to_string(i);
			const std::string newConfigFilePath = Storage::ConfigsDirectory + xorstr_("\\") + newConfigName + xorstr_(".cfg");
			if (!std::filesystem::exists(newConfigFilePath))
			{
				configName = newConfigName;
				configFilePath = newConfigFilePath;

				break;
			}

			i++;
		}
	}

	std::ofstream file(configFilePath);
	file.close();
	
	refresh();

	const auto it = std::find(Configs.begin(), Configs.end(), configName);

	if (it != Configs.end())
		CurrentConfigIndex = std::distance(Configs.begin(), it);

	CurrentConfig = {};
}