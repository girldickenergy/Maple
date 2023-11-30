#pragma once

#include <string>
#include <vector>

#include "EncryptedString.h"
#include "CLR/CLRString.h"

class Spoofer
{
	static inline std::wstring fileMD5;
	static inline std::wstring realClientHash;
	static inline std::wstring realUniqueID;
	static inline std::wstring realUniqueID2;
	static inline std::wstring realUniqueCheck;

	static inline std::wstring currentClientHash;
	static inline std::wstring currentUniqueID;
	static inline std::wstring currentUniqueID2;
	static inline std::wstring currentUniqueCheck;

	static std::string getRandomUninstallID();
	static std::string getRandomDiskID();
	static std::string getRandomAdapters();

	static void refresh();

	static std::string getProfilePathByName(const EncryptedString& profileName);
	static std::string getProfilePathForName(const EncryptedString& profileName);
    static int getProfileIndexByName(const EncryptedString& profileName);
public:
	static inline std::vector<EncryptedString> Profiles;
	static inline int SelectedProfile = 0;
	static inline int LoadedProfile = 0;
	static inline EncryptedString RenamedProfileName;
	static inline EncryptedString NewProfileName;

	static inline bool Initialized = false;

	static void Initialize();
	static void Load();
	static void Delete();
	static void Import();
	static void Export();
	static void Rename();
	static void Create();

	static CLRString* GetClientHash();
	static CLRString* GetUniqueID();
	static CLRString* GetUniqueID2();
	static CLRString* GetUniqueCheck();
};
