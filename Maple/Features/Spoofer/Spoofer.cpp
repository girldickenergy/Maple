#include "Spoofer.h"

#include <combaseapi.h>
#include <time.h>
#include <filesystem>
#include <fstream>

#include "ThemidaSDK.h"
#include "Vanilla.h"
#include "xorstr.hpp"

#include "../../Storage/Storage.h"
#include "../../Storage/StorageConfig.h"
#include "../../SDK/Osu/GameBase.h"
#include "../../Utilities/Crypto/CryptoUtilities.h"
#include "../../Utilities/Clipboard/ClipboardUtilities.h"
#include "../../Utilities/Strings/StringUtilities.h"
#include "../../SDK/Memory.h"
#include "../../Logging/Logger.h"

std::string Spoofer::getRandomUninstallID()
{
	GUID uninstallIDGUID;
	CoCreateGuid(&uninstallIDGUID);

	char uninstallID[39];
	snprintf(uninstallID, sizeof(uninstallID),
		xorstr_("%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x"),
		uninstallIDGUID.Data1, uninstallIDGUID.Data2, uninstallIDGUID.Data3,
		uninstallIDGUID.Data4[0], uninstallIDGUID.Data4[1], uninstallIDGUID.Data4[2], uninstallIDGUID.Data4[3],
		uninstallIDGUID.Data4[4], uninstallIDGUID.Data4[5], uninstallIDGUID.Data4[6], uninstallIDGUID.Data4[7]);

	return uninstallID;
}

std::string Spoofer::getRandomDiskID()
{
	srand(time(0));

	int diskID = 1000000 + (rand() % static_cast<int>(2000000000 - 1000000 + 1));

	return std::to_string(diskID);
}

std::string Spoofer::getRandomAdapters()
{
	srand(time(0));

	std::string adapters;

	char secondNibble[] = { 'A', 'E', '2', '6' };
	char newValArray[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

	int adaptersCount = 2 + (rand() % static_cast<int>(5 - 2 + 1));
	for (int i = 0; i < adaptersCount; i++)
	{
		for (int j = 0; j < 6; j++) {

			adapters += newValArray[rand() % 16];

			if (i == 0)
				adapters += secondNibble[rand() % 4];
			else
				adapters += newValArray[rand() % 16];
		}

		adapters += ".";
	}

	return adapters;
}

void Spoofer::refresh()
{
	Storage::EnsureDirectoryExists(Storage::ProfilesDirectory);

	Profiles.clear();
	Profiles.emplace_back(xorstr_("none"));

	for (const auto& file : std::filesystem::directory_iterator(Storage::ProfilesDirectory))
		if (file.path().extension() == xorstr_(".profile") && Storage::IsValidFileName(file.path().filename().stem().string()))
			Profiles.push_back(file.path().filename().stem().string());
}

void Spoofer::Initialize()
{
	realClientHash = GameBase::GetClientHash();
	realUniqueID = GameBase::GetUniqueID();
	realUniqueID2 = GameBase::GetUniqueID2();
	realUniqueCheck = GameBase::GetUniqueCheck();

	for (const wchar_t c : realClientHash)
	{
		if (c == ':')
			break;
		
		fileMD5 += c;
	}

	refresh();

	const auto it = std::find(Profiles.begin(), Profiles.end(), StorageConfig::DefaultProfile);

	if (it != Profiles.end())
		SelectedProfile = std::distance(Profiles.begin(), it);
	else
		SelectedProfile = 0;

	Load();

	Initialized = true;
}

void Spoofer::Load()
{
	Storage::EnsureDirectoryExists(Storage::ProfilesDirectory);

	if (SelectedProfile == 0)
	{
		currentClientHash = realClientHash;
		currentUniqueID = realUniqueID;
		currentUniqueID2 = realUniqueID2;
		currentUniqueCheck = realUniqueCheck;
	}
	else
	{
		std::ifstream file(Storage::ProfilesDirectory + xorstr_("\\") + Profiles[SelectedProfile] + xorstr_(".profile"));
		std::string line;

		std::wstring currentAdapters;

		while (std::getline(file, line))
		{
			const int delimiterIndex = line.find('=');
			std::string variable = line.substr(0, delimiterIndex);
			std::string value = line.substr(delimiterIndex + 1, std::string::npos);

			if (variable == xorstr_("UninstallID"))
				currentUniqueID = CryptoUtilities::GetMD5Hash(std::wstring(value.begin(), value.end()));

			if (variable == xorstr_("DiskID"))
				currentUniqueID2 = CryptoUtilities::GetMD5Hash(std::wstring(value.begin(), value.end()));

			if (variable == xorstr_("Adapters"))
				currentAdapters = std::wstring(value.begin(), value.end());
		}

		file.close();

		currentUniqueCheck = CryptoUtilities::GetMD5Hash(currentUniqueID + L"8" + L"512" + currentUniqueID2);
		currentClientHash = fileMD5 + L":" + currentAdapters + L":" + CryptoUtilities::GetMD5Hash(currentAdapters) + L":" + CryptoUtilities::GetMD5Hash(currentUniqueID) + L":" + CryptoUtilities::GetMD5Hash(currentUniqueID2) + L":";
	}

	if (!Vanilla::SetCLRString(Memory::Objects[xorstr_("GameBase::ClientHash")], GetClientHash()))
		Logger::Log(LogSeverity::Error, xorstr_("Failed to handle GC of ClientHash!"));
		
	LoadedProfile = SelectedProfile;

	StorageConfig::DefaultProfile = Profiles[LoadedProfile];
	Storage::SaveStorageConfig();
}

void Spoofer::Delete()
{
	Storage::EnsureDirectoryExists(Storage::ProfilesDirectory);

	if (SelectedProfile == 0)
		return;

	const std::string profilePath = Storage::ProfilesDirectory + xorstr_("\\") + Profiles[SelectedProfile] + xorstr_(".profile");

	std::filesystem::remove(profilePath);

	refresh();

	SelectedProfile = 0;

	Load();
}

void Spoofer::Import()
{
	STR_ENCRYPT_START

	Storage::EnsureDirectoryExists(Storage::ProfilesDirectory);

	const std::string encodedProfileData = ClipboardUtilities::Read();

	if (encodedProfileData.empty())
		return;

	const std::string decodedProfileData = CryptoUtilities::MapleXOR(CryptoUtilities::Base64Decode(encodedProfileData), xorstr_("OvpvutSCyRdrx0BF"));
	const std::vector<std::string> decodedProfileDataSplit = StringUtilities::Split(decodedProfileData, "|");

	if (decodedProfileDataSplit.size() < 2 || decodedProfileDataSplit.size() > 2)
		return;

	std::string profileName = CryptoUtilities::Base64Decode(decodedProfileDataSplit[0]);
	const std::string profileData = CryptoUtilities::Base64Decode(decodedProfileDataSplit[1]);

	std::string profileFilePath = Storage::ProfilesDirectory + xorstr_("\\") + profileName + xorstr_(".profile");

	if (!Storage::IsValidFileName(profileName))
		return;

	if (std::filesystem::exists(profileFilePath))
	{
		unsigned int i = 2;
		while (true)
		{
			const std::string newProfileName = profileName + xorstr_("_") + std::to_string(i);
			const std::string newProfileFilePath = Storage::ProfilesDirectory + xorstr_("\\") + newProfileName + xorstr_(".profile");
			if (!std::filesystem::exists(newProfileFilePath))
			{
				profileName = newProfileName;
				profileFilePath = newProfileFilePath;

				break;
			}

			i++;
		}
	}

	std::ofstream ofs(profileFilePath);
	ofs << profileData << std::endl;
	ofs.close();

	refresh();

	const auto it = std::find(Profiles.begin(), Profiles.end(), profileName);

	if (it != Profiles.end())
		SelectedProfile = std::distance(Profiles.begin(), it);

	Load();

	STR_ENCRYPT_END
}

void Spoofer::Export()
{
	STR_ENCRYPT_START

	Storage::EnsureDirectoryExists(Storage::ProfilesDirectory);

	if (SelectedProfile == 0)
		return;

	const std::string profileFilePath = Storage::ProfilesDirectory + xorstr_("\\") + Profiles[SelectedProfile] + xorstr_(".profile");

	std::ifstream ifs(profileFilePath);
	const std::string profileData((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
	ifs.close();

	const std::string encodedProfileData = CryptoUtilities::Base64Encode(CryptoUtilities::MapleXOR(CryptoUtilities::Base64Encode(Profiles[SelectedProfile]) + xorstr_("|") + CryptoUtilities::Base64Encode(profileData), xorstr_("OvpvutSCyRdrx0BF")));

	ClipboardUtilities::Write(encodedProfileData);

	STR_ENCRYPT_END
}

void Spoofer::Rename()
{
	Storage::EnsureDirectoryExists(Storage::ProfilesDirectory);

	if (SelectedProfile == 0)
		return;

	const std::string profileFilePath = Storage::ProfilesDirectory + xorstr_("\\") + Profiles[SelectedProfile] + xorstr_(".profile");

	if (!Storage::IsValidFileName(RenamedProfileName) || Storage::IsSameFileName(RenamedProfileName, Profiles[SelectedProfile]))
		return;

	std::string renamedProfileName = RenamedProfileName;
	std::string renamedProfileFilePath = Storage::ProfilesDirectory + xorstr_("\\") + renamedProfileName + xorstr_(".profile");

	if (std::filesystem::exists(renamedProfileFilePath))
	{
		unsigned int i = 2;
		while (true)
		{
			const std::string newProfileName = renamedProfileName + xorstr_("_") + std::to_string(i);
			const std::string newProfileFilePath = Storage::ProfilesDirectory + xorstr_("\\") + newProfileName + xorstr_(".profile");
			if (!std::filesystem::exists(newProfileFilePath))
			{
				renamedProfileName = newProfileName;
				renamedProfileFilePath = newProfileFilePath;

				break;
			}

			i++;
		}
	}

	std::rename(profileFilePath.c_str(), renamedProfileFilePath.c_str());

	refresh();

	const auto it = std::find(Profiles.begin(), Profiles.end(), renamedProfileName);

	if (it != Profiles.end())
		SelectedProfile = std::distance(Profiles.begin(), it);
}

void Spoofer::Create()
{
	Storage::EnsureDirectoryExists(Storage::ProfilesDirectory);

	std::string profileName = NewProfileName;
	std::string profileFilePath = Storage::ProfilesDirectory + xorstr_("\\") + profileName + xorstr_(".profile");

	if (!Storage::IsValidFileName(profileName))
		return;

	if (std::filesystem::exists(profileFilePath))
	{
		unsigned int i = 2;
		while (true)
		{
			const std::string newProfileName = profileName + xorstr_("_") + std::to_string(i);
			const std::string newProfileFilePath = Storage::ProfilesDirectory + xorstr_("\\") + newProfileName + xorstr_(".profile");
			if (!std::filesystem::exists(newProfileFilePath))
			{
				profileName = newProfileName;
				profileFilePath = newProfileFilePath;

				break;
			}

			i++;
		}
	}

	std::ofstream ofs;
	ofs.open(profileFilePath, std::ofstream::out | std::ofstream::trunc);

	ofs << xorstr_("UninstallID=") << getRandomUninstallID() << std::endl;
	ofs << xorstr_("DiskID=") << getRandomDiskID() << std::endl;
	ofs << xorstr_("Adapters=") << getRandomAdapters() << std::endl;

	ofs.close();

	refresh();

	const auto it = std::find(Profiles.begin(), Profiles.end(), profileName);

	if (it != Profiles.end())
		SelectedProfile = std::distance(Profiles.begin(), it);

	Load();
}

CLRString* __fastcall Spoofer::GetClientHash()
{
	return Vanilla::AllocateCLRString(currentClientHash.c_str());
}

CLRString* Spoofer::GetUniqueID()
{
	return Vanilla::AllocateCLRString(currentUniqueID.c_str());
}

CLRString* Spoofer::GetUniqueID2()
{
	return Vanilla::AllocateCLRString(currentUniqueID2.c_str());
}

CLRString* Spoofer::GetUniqueCheck()
{
	return Vanilla::AllocateCLRString(currentUniqueCheck.c_str());
}
