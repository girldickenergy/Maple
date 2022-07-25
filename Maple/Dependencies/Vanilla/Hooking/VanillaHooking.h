#pragma once

#include <string>
#include <vector>

#include "VanillaHook.h"
#include "VanillaResult.h"

typedef void* (__fastcall* fnBaseThreadInitThunk)(ULONG Unknown, PVOID StartAddress, PVOID ThreadParameter);

class VanillaHooking
{
	static inline std::vector<VanillaHook*> hooks;
	static VanillaHook* findHook(const std::string& name);
	static void removeHook(const std::string& name);

	typedef void* (__fastcall* fnBaseThreadInitThunk)(ULONG Unknown, PVOID StartAddress, PVOID ThreadParameter);
	static inline PVOID exceptionHandlerHandle = reinterpret_cast<PVOID>(0x00DE00FF);
	static inline fnBaseThreadInitThunk m_oBaseThreadInitThunk = reinterpret_cast<fnBaseThreadInitThunk>(0x00000000);
	static LONG __stdcall hwbrkHandler(PEXCEPTION_POINTERS ExceptionInfo);
	static void* __fastcall hkBaseThreadInitThunk(ULONG Unknown, PVOID StartAddress, PVOID ThreadParameter);

	static VanillaResult installHWBPHook(uintptr_t functionAddress, HANDLE hookThread, HANDLE hThread = nullptr);

	static std::vector<uint8_t> getFunctionPrologue(uintptr_t functionAddress, unsigned int minimumBytes = 5);
	static uintptr_t installTrampoline(uintptr_t functionAddress, const std::vector<uint8_t>& functionPrologue);
	static void relocateRelativeAddresses(uintptr_t oldLocation, uintptr_t newLocation, unsigned int length);
	static uintptr_t installInlineHook(uintptr_t functionAddress, uintptr_t detourFunctionAddress, const std::vector<uint8_t>& functionPrologue);
public:
	static VanillaResult InstallHook(const std::string& name, uintptr_t functionAddress, uintptr_t detourFunctionAddress, uintptr_t* originalFunction, VanillaHookType type = VanillaHookType::Inline);
	static VanillaResult UninstallHook(const std::string& name);
	static void UninstallAllHooks();
};
