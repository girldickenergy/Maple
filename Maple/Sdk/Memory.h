#pragma once

#include <map>
#include <mutex>
#include <string>
#include <vector>

struct MaplePattern
{
	std::string Pattern;
	unsigned int Offset;
	unsigned int ReadCount;

	MaplePattern(const std::string& pattern, unsigned int offset, unsigned int readCount)
	{
		Pattern = pattern;
		Offset = offset;
		ReadCount = readCount;
	}

	MaplePattern() = default;
};

struct MaplePatch
{
	std::string Name;
	std::string Pattern;
	unsigned int ScanSize;
	unsigned int Offset;
	std::vector<uint8_t> Patch;

	MaplePatch(const std::string& name, const std::string& pattern, unsigned int scanSize, unsigned int offset, const std::vector<uint8_t>& patch)
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
	std::string Name;
	uintptr_t DetourFunctionAddress;
	uintptr_t* OriginalFunction;

	MapleHook(const std::string& name, uintptr_t detourFunctionAddress, uintptr_t* originalFunction)
	{
		Name = name;
		DetourFunctionAddress = detourFunctionAddress;
		OriginalFunction = originalFunction;
	}

	MapleHook() = default;
};

class Memory
{
	static inline std::map<std::string, MaplePattern> pendingObjects;
	static inline std::map<std::string, MaplePatch> pendingPatches;
	static inline std::map<std::string, MapleHook> pendingHooks;

	static inline std::mutex mutex;
	static void jitCallback(uintptr_t address, unsigned int size);

	static inline bool initialized = false;
public:
	static inline std::map<std::string, uintptr_t> Objects;

	static void StartInitialize();
	static void EndInitialize();

	static void AddObject(const std::string& name, const std::string& pattern, unsigned int offset = 0, unsigned int readCount = 0);
	static void AddPatch(const std::string& name, const std::string& objectName, const std::string& pattern, unsigned int scanSize, unsigned int offset, const std::vector<uint8_t>& patch);
	static void AddHook(const std::string& name, const std::string& objectName, uintptr_t detourFunctionAddress, uintptr_t* originalFunction);
};
