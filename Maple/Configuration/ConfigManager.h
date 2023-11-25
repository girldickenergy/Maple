#pragma once

#include <vector>

#include "Config.h"
#include "EncryptedString.h"

class ConfigManager
{
	static void refresh();
	static std::string getConfigPathByName(const EncryptedString& configName);
    static std::string getUniqueConfigPath();
    static int getConfigIndexByName(const EncryptedString& configName);

public:
	static inline std::vector<EncryptedString> Configs;
	static inline Config CurrentConfig;
	static inline int CurrentConfigIndex = 0;
    static inline EncryptedString RenamedConfigName;
	static inline EncryptedString NewConfigName;

	static inline bool ForceDisableScoreSubmission = false;
	static inline bool BypassFailed = false;

	static void Initialize();
	static void Load();
	static void Save();
	static void Delete();
	static void Import();
	static void Export();
	static void Rename();
	static void Create();
};
