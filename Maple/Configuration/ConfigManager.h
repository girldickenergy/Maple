#pragma once

#include <string>
#include <vector>

#include "Config.h"

class ConfigManager
{
	static void refresh();
public:
	static inline std::vector<std::string> Configs;
	static inline Config CurrentConfig;
	static inline int CurrentConfigIndex = 0;
	static inline char RenamedConfigName[32] = { };
	static inline char NewConfigName[32] = { };

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