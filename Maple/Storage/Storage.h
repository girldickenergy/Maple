#pragma once

#include <string>

class Storage
{
	static inline std::string storageConfigFilepath;

	static std::string encryptEntry(const std::string& key, const std::string& value);
	static std::string decryptEntry(const std::string& entry);

	static void loadStorageConfig();
public:
	static inline std::string StorageDirectory;
	static inline std::string LogsDirectory;
	static inline std::string ConfigsDirectory;
	static inline std::string ProfilesDirectory;

	static void Initialize(const std::string& uniqueName);
	static bool IsSameFileName(const std::string& a, const std::string& b);
	static bool IsValidFileName(const std::string& filename);
	static void EnsureDirectoryExists(const std::string& directory);
	static void SaveStorageConfig();
};