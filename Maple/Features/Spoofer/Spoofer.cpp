#include "Spoofer.h"

#include <time.h>
#include <filesystem>
#include <fstream>

#include "VirtualizerSDK.h"
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
	{
		if (file.path().extension() == xorstr_(".profile"))
		{
            EncryptedString profileName;
			profileName.Deserialize(CryptoUtilities::Base32Decode(file.path().filename().stem().string()));

			if (profileName != xorstr_("none"))
				Profiles.push_back(profileName);
		}
	}

	std::sort(Profiles.begin() + 1, Profiles.end(), [](const EncryptedString& a, const EncryptedString& b) -> bool
    {
        return a < b;
    });
}

std::string Spoofer::getProfilePathByName(const EncryptedString& profileName)
{
	Storage::EnsureDirectoryExists(Storage::ProfilesDirectory);

	for (const auto& file : std::filesystem::directory_iterator(Storage::ProfilesDirectory))
	{
		if (file.path().extension() == xorstr_(".profile"))
		{
            EncryptedString profileFileName;
			profileFileName.Deserialize(CryptoUtilities::Base32Decode(file.path().filename().stem().string()));

			if (profileFileName == profileName)
                return file.path().string();
		}
	}

	return {};
}

std::string Spoofer::getProfilePathForName(const EncryptedString& profileName)
{
    Storage::EnsureDirectoryExists(Storage::ProfilesDirectory);

	return Storage::ProfilesDirectory + xorstr_("\\") + CryptoUtilities::Base32Encode(profileName.Serialize()) + xorstr_(".profile");
}

int Spoofer::getProfileIndexByName(const EncryptedString& profileName)
{
    for (int i = 0; i < Profiles.size(); i++)
        if (Profiles[i] == profileName)
            return i;

	return 0;
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

	SelectedProfile = getProfileIndexByName(Storage::Config.DefaultProfile);

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
		const std::string profileFilePath = getProfilePathByName(Profiles[SelectedProfile]);
	    if (profileFilePath.empty() || !std::filesystem::exists(profileFilePath))
	    {
	        refresh();

	        SelectedProfile = 0;

			Load();

	        return;
		}

		std::ifstream profileFile(profileFilePath, std::ifstream::binary);

		EncryptedString uninstallId;
        EncryptedString diskId;
        EncryptedString adapters;

		uninstallId.Deserialize(profileFile);
        diskId.Deserialize(profileFile);
        adapters.Deserialize(profileFile);

		char uninstallIdBuf[uninstallId.GetSize()];
        char diskIdBuf[diskId.GetSize()];
        char adaptersBuf[adapters.GetSize()];

		uninstallId.GetData(uninstallIdBuf);
        diskId.GetData(diskIdBuf);
        adapters.GetData(adaptersBuf);

		std::string uninstallIdStr(uninstallIdBuf);
        std::string diskIdStr(diskIdBuf);
        std::string adaptersStr(adaptersBuf);

		currentUniqueID = CryptoUtilities::GetMD5Hash(std::wstring(uninstallIdStr.begin(), uninstallIdStr.end()));
		currentUniqueID2 = CryptoUtilities::GetMD5Hash(std::wstring(diskIdStr.begin(), diskIdStr.end()));
        std::wstring currentAdapters = std::wstring(adaptersStr.begin(), adaptersStr.end());

		profileFile.close();

		currentUniqueCheck = CryptoUtilities::GetMD5Hash(currentUniqueID + L"8" + L"512" + currentUniqueID2);
		currentClientHash = fileMD5 + L":" + currentAdapters + L":" + CryptoUtilities::GetMD5Hash(currentAdapters) + L":" + CryptoUtilities::GetMD5Hash(currentUniqueID) + L":" + CryptoUtilities::GetMD5Hash(currentUniqueID2) + L":";
	}

	if (!Vanilla::SetCLRString(Memory::Objects[xorstr_("GameBase::ClientHash")], GetClientHash()))
		Logger::Log(LogSeverity::Error, xorstr_("Failed to handle GC of ClientHash!"));
		
	LoadedProfile = SelectedProfile;

	Storage::Config.DefaultProfile = Profiles[LoadedProfile];
	Storage::SaveStorageConfig();
}

void Spoofer::Delete()
{
	Storage::EnsureDirectoryExists(Storage::ProfilesDirectory);

	if (SelectedProfile == 0)
		return;

	const std::string profilePath = getProfilePathByName(Profiles[SelectedProfile]);
    if (!profilePath.empty())
		std::filesystem::remove(profilePath);

	const EncryptedString lastLoadedProfile = Profiles[LoadedProfile];

	refresh();

	SelectedProfile = 0;
    LoadedProfile = getProfileIndexByName(lastLoadedProfile);

	Load();
}

void Spoofer::Import()
{
	Storage::EnsureDirectoryExists(Storage::ProfilesDirectory);

	const std::string encodedProfileData = ClipboardUtilities::Read();

	if (encodedProfileData.empty())
		return;

	const std::string decodedProfileData = CryptoUtilities::Base64Decode(encodedProfileData);
	const std::vector<std::string> decodedProfileDataSplit = StringUtilities::Split(decodedProfileData, "|||");

	if (decodedProfileDataSplit.size() < 2 || decodedProfileDataSplit.size() > 2)
		return;

	EncryptedString profileName;
	profileName.Deserialize(CryptoUtilities::Base32Decode(decodedProfileDataSplit[0]));

	if (profileName == xorstr_("none"))
		return;

	const std::vector<uint8_t> profileData = CryptoUtilities::Base64DecodeToBytes(decodedProfileDataSplit[1]);

	if (!getProfilePathByName(profileName).empty())
	{
        int i = 1;
		while (!getProfilePathByName(profileName + xorstr_("(") + std::to_string(i).c_str() + xorstr_(")")).empty())
			i++;

		// todo: fix += operator?
		profileName = profileName + xorstr_("(") + std::to_string(i).c_str() + xorstr_(")");
	}

	std::ofstream configFile(getProfilePathForName(profileName), std::ofstream::out | std::ofstream::binary | std::ofstream::trunc);
	configFile.write(reinterpret_cast<const char*>(profileData.data()), profileData.size());
	configFile.close();

	refresh();

	SelectedProfile = getProfileIndexByName(profileName);

	Load();
}

void Spoofer::Export()
{
	Storage::EnsureDirectoryExists(Storage::ProfilesDirectory);

	if (SelectedProfile == 0)
		return;

	const std::string profileFilePath = getProfilePathByName(Profiles[SelectedProfile]);

	std::ifstream profileFile(profileFilePath, std::ifstream::binary);
	const std::vector<uint8_t> profileData((std::istreambuf_iterator<char>(profileFile)), std::istreambuf_iterator<char>());
	profileFile.close();

	ClipboardUtilities::Write(CryptoUtilities::Base64Encode(CryptoUtilities::Base32Encode(Profiles[SelectedProfile].Serialize()) + xorstr_("|||") + CryptoUtilities::Base64Encode(profileData)));
}

void Spoofer::Rename()
{
	Storage::EnsureDirectoryExists(Storage::ProfilesDirectory);

	if (SelectedProfile == 0)
		return;

	if (RenamedProfileName == xorstr_("none") || RenamedProfileName == Profiles[SelectedProfile])
		return;

	EncryptedString renamedProfileName = RenamedProfileName;

	if (!getProfilePathByName(renamedProfileName).empty())
	{
        int i = 1;
		while (!getProfilePathByName(renamedProfileName + xorstr_("(") + std::to_string(i).c_str() + xorstr_(")")).empty())
			i++;

		// todo: fix += operator?
		renamedProfileName = renamedProfileName + xorstr_("(") + std::to_string(i).c_str() + xorstr_(")");
	}

	std::rename(getProfilePathByName(Profiles[SelectedProfile]).c_str(), getProfilePathForName(renamedProfileName).c_str());

	const EncryptedString lastLoadedProfile = SelectedProfile == LoadedProfile ? renamedProfileName : Profiles[LoadedProfile];

	refresh();

    SelectedProfile = getProfileIndexByName(renamedProfileName);
    LoadedProfile = getProfileIndexByName(lastLoadedProfile);
}

void Spoofer::Create()
{
	Storage::EnsureDirectoryExists(Storage::ProfilesDirectory);

	if (NewProfileName == xorstr_("none"))
        return;

	EncryptedString newProfileName = NewProfileName;

	if (!getProfilePathByName(newProfileName).empty())
	{
        int i = 1;
		while (!getProfilePathByName(newProfileName + xorstr_("(") + std::to_string(i).c_str() + xorstr_(")")).empty())
			i++;

		// todo: fix += operator?
		newProfileName = newProfileName + xorstr_("(") + std::to_string(i).c_str() + xorstr_(")");
	}

	std::ofstream profileFile(getProfilePathForName(newProfileName), std::ofstream::out | std::ofstream::trunc | std::ofstream::binary);

    EncryptedString(getRandomUninstallID().c_str()).Serialize(profileFile);
    EncryptedString(getRandomDiskID().c_str()).Serialize(profileFile);
    EncryptedString(getRandomAdapters().c_str()).Serialize(profileFile);

    profileFile.close();

	refresh();

    SelectedProfile = getProfileIndexByName(newProfileName);

	Load();
}

[[clang::optnone]] CLRString* Spoofer::GetClientHash()
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
