#pragma once

#include <string>

#include "StorageConfig.h"

class Storage
{
	static inline std::string storageConfigFilepath;

public:
    static inline StorageConfig Config;
	static inline std::string StorageDirectory;
	static inline std::string LogsDirectory;
	static inline std::string ConfigsDirectory;
	static inline std::string ProfilesDirectory;

	static void Initialize(const std::string& uniqueName);
	static void EnsureDirectoryExists(const std::string& directory);
	static void SaveStorageConfig();
};
