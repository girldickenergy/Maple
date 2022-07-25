#pragma once

#include "CLR/CLRString.h"
#include "VanillaResult.h"

class Vanilla
{
	typedef int(__stdcall* fnCompileMethod)(uintptr_t thisvar, uintptr_t compHnd, uintptr_t methodInfo, unsigned int flags, uintptr_t* entryAddress, unsigned int* nativeSizeOfCode);
	typedef void(__cdecl* fnJITCallback)(uintptr_t functionAddress, unsigned int functionSize);
	static inline fnCompileMethod oCompileMethod;
	static int __stdcall compileMethodHook(uintptr_t thisvar, uintptr_t compHnd, uintptr_t methodInfo, unsigned int flags, uintptr_t* entryAddress, unsigned int* nativeSizeOfCode);

	typedef CLRString* (__cdecl* fnAllocateCLRString)(const wchar_t* pwsz);
	static inline fnAllocateCLRString allocateCLRString;

	static inline bool usingCLR = false;
	static inline fnJITCallback jitCallback = nullptr;
public:
	static VanillaResult Initialize(bool useCLR = false);
	static void Shutdown();
	
	static void SetJITCallback(void* callback);
	static void RemoveJITCallback();
	static CLRString* AllocateCLRString(const wchar_t* pwsz);

	static bool CheckAddressInModule(uintptr_t address, const std::string& moduleName);
};
