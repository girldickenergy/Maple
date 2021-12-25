#include "Spoofer.h"

#include <time.h>
#include <filesystem>
#include <fstream>

#include "../../Sdk/Osu/GameBase.h"
#include "../../Utilities/Crypto/CryptoHelper.h"
#include "../../Utilities/Directories/DirectoryHelper.h"

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

void Spoofer::saveConfigFile()
{
	DirectoryHelper::EnsureDirectoriesExist();

	std::ofstream ofs;
	ofs.open(configFilepath, std::ofstream::out | std::ofstream::trunc);

	ofs << "CurrentProfile=" << Profiles[SelectedProfile] << std::endl;
	
	ofs.close();
}

void Spoofer::loadConfigFile()
{
	DirectoryHelper::EnsureDirectoriesExist();

	if (!std::filesystem::exists(configFilepath))
	{
		SelectedProfile = 0;

		return;
	}

	std::ifstream file(configFilepath);
	std::string line;

	while (std::getline(file, line))
	{
		const int delimiterIndex = line.find('=');
		std::string variable = line.substr(0, delimiterIndex);
		std::string value = line.substr(delimiterIndex + 1, std::string::npos);

		if (variable == "CurrentProfile")
		{
			if (isValidName(value))
			{
				const auto it = std::find(Profiles.begin(), Profiles.end(), value);

				if (it != Profiles.end())
					SelectedProfile = std::distance(Profiles.begin(), it);
				else
					SelectedProfile = 0;
			}
			else
				SelectedProfile = 0;
		}
	}

	file.close();
}

void Spoofer::refresh()
{
	DirectoryHelper::EnsureDirectoriesExist();

	Profiles.clear();
	Profiles.emplace_back("none");

	for (const auto& file : std::filesystem::directory_iterator(DirectoryHelper::ProfilesDirectory))
		if (file.path().extension() == ".profile" && isValidName(file.path().filename().stem().string()))
			Profiles.push_back(file.path().filename().stem().string());
}

void Spoofer::updateCOM()
{
	TypeExplorer obfuscatedStringType = GameBase::RawGameBase["UniqueId"].Field.GetTypeUnsafe();

	Method obfuscatedStringSetValue = obfuscatedStringType["set_Value"].Method;
	Field obfuscatedStringChangesField = obfuscatedStringType["c"].Field;

	variant_t uniqueIdInstance = GameBase::RawGameBase["UniqueId"].Field.GetValueUnsafe(variant_t());
	variant_t uniqueId2Instance = GameBase::RawGameBase["UniqueId2"].Field.GetValueUnsafe(variant_t());
	variant_t uniqueCheckInstance = GameBase::RawGameBase["UniqueCheck"].Field.GetValueUnsafe(variant_t());

	GameBase::RawGameBase["ClientHash"].Field.SetValueUnsafe(variant_t(), variant_t(currentClientHash.c_str()));

	std::vector<variant_t> args;
	
	args.push_back(variant_t(currentUniqueID.c_str()));
	obfuscatedStringSetValue.InvokeUnsafe(uniqueIdInstance, args);
	obfuscatedStringChangesField.SetValueUnsafe(uniqueIdInstance, variant_t(obfuscatedStringChangesField.GetValueUnsafe(uniqueIdInstance).intVal - 1));

	args.clear();
	args.push_back(variant_t(currentUniqueID2.c_str()));
	obfuscatedStringSetValue.InvokeUnsafe(uniqueId2Instance, args);
	obfuscatedStringChangesField.SetValueUnsafe(uniqueId2Instance, variant_t(obfuscatedStringChangesField.GetValueUnsafe(uniqueId2Instance).intVal - 1));

	args.clear();
	args.push_back(variant_t(currentUniqueCheck.c_str()));
	obfuscatedStringSetValue.InvokeUnsafe(uniqueCheckInstance, args);
	obfuscatedStringChangesField.SetValueUnsafe(uniqueCheckInstance, variant_t(obfuscatedStringChangesField.GetValueUnsafe(uniqueCheckInstance).intVal - 1));
}

void Spoofer::Initialize()
{
	configFilepath = DirectoryHelper::ProfilesDirectory + "\\" + Communication::CurrentUser->UsernameHashed + ".cfg";

	realClientHash = GameBase::GetClientHash();
	realUniqueID = GameBase::GetUniqueID();
	realUniqueID2 = GameBase::GetUniqueID2();
	realUniqueCheck = GameBase::GetUniqueCheck();

	for (int i = 0; i < realClientHash.size(); i++)
	{
		if (realClientHash[i] == ':')
			break;

		fileMD5 += realClientHash[i];
	}

	refresh();
	loadConfigFile();
	Load();
}

void Spoofer::Load()
{
	DirectoryHelper::EnsureDirectoriesExist();

	if (SelectedProfile == 0)
	{
		currentClientHash = realClientHash;
		currentUniqueID = realUniqueID;
		currentUniqueID2 = realUniqueID2;
		currentUniqueCheck = realUniqueCheck;
	}
	else
	{
		std::ifstream file(DirectoryHelper::ProfilesDirectory + "\\" + Profiles[SelectedProfile] + ".profile");
		std::string line;

		std::wstring currentAdapters;

		while (std::getline(file, line))
		{
			const int delimiterIndex = line.find('=');
			std::string variable = line.substr(0, delimiterIndex);
			std::string value = line.substr(delimiterIndex + 1, std::string::npos);

			if (variable == "UninstallID")
				currentUniqueID = CryptoHelper::GetMD5Hash(std::wstring(value.begin(), value.end()));

			if (variable == "DiskID")
				currentUniqueID2 = CryptoHelper::GetMD5Hash(std::wstring(value.begin(), value.end()));

			if (variable == "Adapters")
				currentAdapters = std::wstring(value.begin(), value.end());
		}

		file.close();

		currentUniqueCheck = CryptoHelper::GetMD5Hash(currentUniqueID + L"8" + L"512" + currentUniqueID2);
		currentClientHash = fileMD5 + L":" + currentAdapters + L":" + CryptoHelper::GetMD5Hash(currentAdapters) + L":" + CryptoHelper::GetMD5Hash(currentUniqueID) + L":" + CryptoHelper::GetMD5Hash(currentUniqueID2) + L":";
	}

	updateCOM();

	LoadedProfile = SelectedProfile;

	saveConfigFile();
}

void Spoofer::Delete()
{
	DirectoryHelper::EnsureDirectoriesExist();

	if (SelectedProfile == 0)
		return;

	const std::string profilePath = DirectoryHelper::ProfilesDirectory + "\\" + Profiles[SelectedProfile] + ".profile";

	std::filesystem::remove(profilePath);

	refresh();

	SelectedProfile = 0;

	Load();
}

void Spoofer::Create()
{
	DirectoryHelper::EnsureDirectoriesExist();

	const std::string profilePath = DirectoryHelper::ProfilesDirectory + "\\" + NewProfileName + ".profile";

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

void Spoofer::Update()
{
	GameBase::SetClientHash(currentClientHash);
	GameBase::SetUniqueID(currentUniqueID);
	GameBase::SetUniqueID2(currentUniqueID2);
	GameBase::SetUniqueCheck(currentUniqueCheck);
}
