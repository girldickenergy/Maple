#pragma once

#include <string>

class Storage
{
	static inline std::string storageConfigFilepath;

	static bool isSameName(const std::string& a, const std::string& b);
	static void loadStorageConfig();
public:
	static inline std::string StorageDirectory;
	static inline std::string LogsDirectory;
	static inline std::string ConfigsDirectory;
	static inline std::string ProfilesDirectory;

	static void Initialize(const std::string& uniqueName);
	static bool IsValidFileName(const std::string& filename);
	static void EnsureDirectoryExists(const std::string& directory);
	static void SaveStorageConfig();
};