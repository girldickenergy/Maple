#pragma once

#include <mutex>
#include <vector>

#include "CLR/CLRString.h"
#include "VanillaResult.h"

class Vanilla
{
	typedef int(__stdcall* fnCompileMethod)(uintptr_t thisvar, uintptr_t compHnd, uintptr_t methodInfo, unsigned int flags, uintptr_t* entryAddress, unsigned int* nativeSizeOfCode);
	typedef void(__cdecl* fnJITCallback)(uintptr_t functionAddress, unsigned int functionSize);
	static inline fnCompileMethod oCompileMethod;
	static int __stdcall compileMethodHook(uintptr_t thisvar, uintptr_t compHnd, uintptr_t methodInfo, unsigned int flags, uintptr_t* entryAddress, unsigned int* nativeSizeOfCode);

	static inline std::mutex mutex;
	static inline std::vector<std::reference_wrapper<std::uintptr_t>> relocations;

	typedef void(__stdcall* fnRelocateAddress)(uint8_t** block);
	static inline fnRelocateAddress oRelocateAddress;
	static void __stdcall relocateAddressHook(uint8_t** block);

	typedef CLRString* (__cdecl* fnAllocateCLRString)(const wchar_t* pwsz);
	static inline fnAllocateCLRString allocateCLRString;

	static inline bool usingCLR = false;
	static inline fnJITCallback jitCallback = nullptr;
public:
	static VanillaResult Initialize(bool useCLR = false);
	static void Shutdown();
	
	static void SetJITCallback(void* callback);
	static void RemoveJITCallback();
	static void AddRelocation(std::reference_wrapper<std::uintptr_t> relocation);
	static void RemoveRelocation(std::reference_wrapper<std::uintptr_t> relocation);
	static CLRString* AllocateCLRString(const wchar_t* pwsz);

	static bool CheckAddressInModule(uintptr_t address, const std::string& moduleName);
};
