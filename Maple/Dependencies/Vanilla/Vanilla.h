#pragma once

#pragma comment(lib, "mscoree.lib")
#include "mscorlib.h"

#include <map>
#include <vector>

#include "COM/Assembly.h"
#include "Explorer/Explorer.h"
#include "Patching/Patch.h"

struct MemoryRegion
{
	DWORD BaseAddress;
	SIZE_T RegionSize;
	DWORD State;
	DWORD Protect;
	DWORD Type;

	MemoryRegion(MEMORY_BASIC_INFORMATION32 mbi)
	{
		BaseAddress = mbi.BaseAddress;
		RegionSize = mbi.RegionSize;
		State = mbi.State;
		Protect = mbi.Protect;
		Type = mbi.Type;
	}
};

class Vanilla
{
	static inline std::vector<MemoryRegion> memoryRegions;
	static void cacheMemoryRegions();

	static inline std::map<std::string, Patch> patches;

	static void initializeCOM();
	static void populateAssemblies();
public:
	static inline mscorlib::_AppDomainPtr DefaultDomain = NULL;

	static inline Assembly OsuStubAssembly;
	static inline Assembly OsuAssembly;
	static inline Assembly MscorlibAssembly;

	static inline ::Explorer Explorer;
	
	static void Initialize();

	static uintptr_t FindSignature(const char* pattern, const char* mask);
	static uintptr_t FindSignature(const char* signature, const char* mask, uintptr_t entryPoint, int size);
	static DWORD GetModuleSize(const char* moduleName);

	static bool InstallPatch(const std::string& name, uintptr_t entryPoint, const char* signature, const char* mask, int size, int offset, const char* patch);
	static bool InstallNOPPatch(const std::string& name, uintptr_t entryPoint, const char* signature, const char* mask, int size, int offset, int nopSize);
	static void RemovePatch(const std::string& name);
	static void RemoveAllPatches();
};