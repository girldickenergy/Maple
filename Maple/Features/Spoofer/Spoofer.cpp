#include "Spoofer.h"

#include <combaseapi.h>
#include <time.h>
#include <filesystem>
#include <fstream>

#include "Vanilla.h"
#include "../../Storage/Storage.h"
#include "../../Storage/StorageConfig.h"
#include "../../SDK/Osu/GameBase.h"
#include "../../Utilities/Crypto/CryptoUtilities.h"
#include "../../SDK/Online/BanchoClient.h"

std::string Spoofer::getRandomUninstallID()
{
	GUID uninstallIDGUID;
	CoCreateGuid(&uninstallIDGUID);

	char uninstallID[39];
	snprintf(uninstallID, sizeof(uninstallID),
		"%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x",
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

bool Spoofer::isSameName(const std::string& a, const std::string& b)
{
	std::string aLowerCase = a;
	std::string bLowerCase = b;
	std::transform(aLowerCase.begin(), aLowerCase.end(), aLowerCase.begin(), tolower);
	std::transform(bLowerCase.begin(), bLowerCase.end(), bLowerCase.begin(), tolower);

	return aLowerCase == bLowerCase;
}

bool Spoofer::isValidName(const std::string& name)
{
	return !name.empty() && !isSameName(name, "none");
}

void Spoofer::refresh()
{
	Storage::EnsureDirectoryExists(Storage::ProfilesDirectory);

	Profiles.clear();
	Profiles.emplace_back("none");

	for (const auto& file : std::filesystem::directory_iterator(Storage::ProfilesDirectory))
		if (file.path().extension() == ".profile" && isValidName(file.path().filename().stem().string()))
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
		std::ifstream file(Storage::ProfilesDirectory + "\\" + Profiles[SelectedProfile] + ".profile");
		std::string line;

		std::wstring currentAdapters;

		while (std::getline(file, line))
		{
			const int delimiterIndex = line.find('=');
			std::string variable = line.substr(0, delimiterIndex);
			std::string value = line.substr(delimiterIndex + 1, std::string::npos);

			if (variable == "UninstallID")
				currentUniqueID = CryptoUtilities::GetMD5Hash(std::wstring(value.begin(), value.end()));

			if (variable == "DiskID")
				currentUniqueID2 = CryptoUtilities::GetMD5Hash(std::wstring(value.begin(), value.end()));

			if (variable == "Adapters")
				currentAdapters = std::wstring(value.begin(), value.end());
		}

		file.close();

		currentUniqueCheck = CryptoUtilities::GetMD5Hash(currentUniqueID + L"8" + L"512" + currentUniqueID2);
		currentClientHash = fileMD5 + L":" + currentAdapters + L":" + CryptoUtilities::GetMD5Hash(currentAdapters) + L":" + CryptoUtilities::GetMD5Hash(currentUniqueID) + L":" + CryptoUtilities::GetMD5Hash(currentUniqueID2) + L":";
	}

	BanchoClient::InitializePrivate();

	LoadedProfile = SelectedProfile;

	StorageConfig::DefaultProfile = Profiles[LoadedProfile];
	Storage::SaveStorageConfig();
}

void Spoofer::Delete()
{
	Storage::EnsureDirectoryExists(Storage::ProfilesDirectory);

	if (SelectedProfile == 0)
		return;

	const std::string profilePath = Storage::ProfilesDirectory + "\\" + Profiles[SelectedProfile] + ".profile";

	std::filesystem::remove(profilePath);

	refresh();

	SelectedProfile = 0;

	Load();
}

void Spoofer::Create()
{
	Storage::EnsureDirectoryExists(Storage::ProfilesDirectory);

	const std::string profilePath = Storage::ProfilesDirectory + "\\" + NewProfileName + ".profile";

	if (!isValidName(NewProfileName) || std::filesystem::exists(profilePath))
		return;

	std::ofstream ofs;
	ofs.open(profilePath, std::ofstream::out | std::ofstream::trunc);

	ofs << "UninstallID=" << getRandomUninstallID() << std::endl;
	ofs << "DiskID=" << getRandomDiskID() << std::endl;
	ofs << "Adapters=" << getRandomAdapters() << std::endl;

	ofs.close();

	refresh();

	const auto it = std::find(Profiles.begin(), Profiles.end(), NewProfileName);

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
