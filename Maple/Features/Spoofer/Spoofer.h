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

	static void refresh();
public:
	static inline std::vector<std::string> Profiles;
	static inline int SelectedProfile = 0;
	static inline int LoadedProfile = 0;
	static inline char RenamedProfileName[24];
	static inline char NewProfileName[24];

	static inline bool Initialized = false;

	static void Initialize();
	static void Load();
	static void Delete();
	static void Import();
	static void Export();
	static void Rename();
	static void Create();

	static CLRString* __fastcall GetClientHash();
	static CLRString* GetUniqueID();
	static CLRString* GetUniqueID2();
	static CLRString* GetUniqueCheck();
};
