#pragma once

#include <map>
#include <mutex>
#include <string>
#include <vector>

#include "EncryptedString.h"

struct MaplePattern
{
	std::string Pattern;
	unsigned int Offset;
	unsigned int ReadCount;
	bool ResolveRelativeAddress;

	MaplePattern(const std::string& pattern, unsigned int offset, unsigned int readCount, bool resolveRelativeAddress)
	{
		Pattern = pattern;
		Offset = offset;
		ReadCount = readCount;
		ResolveRelativeAddress = resolveRelativeAddress;
	}

	MaplePattern() = default;
};

struct MaplePatch
{
	EncryptedString Name;
	std::string Pattern;
	unsigned int ScanSize;
	unsigned int Offset;
	std::vector<uint8_t> Patch;

	MaplePatch(const char* name, const std::string& pattern, unsigned int scanSize, unsigned int offset, const std::vector<uint8_t>& patch)
	{
		Name = name;
		Pattern = pattern;
		ScanSize = scanSize;
		Offset = offset;
		Patch = patch;
	}

	MaplePatch() = default;
};

struct MapleHook
{
	EncryptedString Name;
	uintptr_t DetourFunctionAddress;
	uintptr_t* OriginalFunction;
	bool Safe;

	MapleHook(const char* name, uintptr_t detourFunctionAddress, uintptr_t* originalFunction, bool safe)
	{
		Name = name;
		DetourFunctionAddress = detourFunctionAddress;
		OriginalFunction = originalFunction;
		Safe = safe;
	}

	MapleHook() = default;
};

class Memory
{
	static inline std::map<EncryptedString, MaplePattern> pendingObjects;
	static inline std::map<EncryptedString, MaplePatch> pendingPatches;
	static inline std::map<EncryptedString, MapleHook> pendingHooks;

	static inline std::mutex mutex;
	static void jitCallback(uintptr_t address, unsigned int size);

	static inline bool initialized = false;
public:
	static inline std::map<EncryptedString, uintptr_t> Objects;

	static void StartInitialize();
	static void EndInitialize();

	static void AddObject(const char* name, const char* pattern, unsigned int offset = 0, unsigned int readCount = 0, bool resolveRelativeAddress = false);
	static void AddPatch(const char* name, const char* objectName, const char* pattern, unsigned int scanSize, unsigned int offset, const std::vector<uint8_t>& patch);
	static void AddHook(const char* name, const char* objectName, uintptr_t detourFunctionAddress, uintptr_t* originalFunction, bool safe = false);
};
