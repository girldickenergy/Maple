#pragma once

#include <string>
#include <vector>

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

	static bool isSameName(const std::string& a, const std::string& b);
	static bool isValidName(const std::string& name);
	static void refresh();
public:
	static inline std::vector<std::string> Profiles;
	static inline int SelectedProfile = 0;
	static inline int LoadedProfile = 0;
	static inline char NewProfileName[24];

	static inline bool Initialized = false;

	static void Initialize();
	static void Load();
	static void Delete();
	static void Create();

	static CLRString* __fastcall GetClientHash();
	static CLRString* GetUniqueID();
	static CLRString* GetUniqueID2();
	static CLRString* GetUniqueCheck();
};
