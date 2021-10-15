#pragma once

#include <windows.h>
#include <string>
#include <vector>

#include "CinnamonResult.h"
#include "Hook.h"

typedef void* (__fastcall* fnBaseThreadInitThunk)(ULONG Unknown, PVOID StartAddress, PVOID ThreadParameter);

class Cinnamon
{
	static LONG __stdcall hwbrkHandler(PEXCEPTION_POINTERS ExceptionInfo);
	static inline PVOID exceptionHandlerHandle = reinterpret_cast<PVOID>(0x00DE00FF);
	
	static void* __fastcall hkBaseThreadInitThunk(ULONG Unknown, PVOID StartAddress, PVOID ThreadParameter);
	static inline fnBaseThreadInitThunk m_oBaseThreadInitThunk = reinterpret_cast<fnBaseThreadInitThunk>(0x00000000);

	static inline std::vector<Hook*> hooks;

	static Hook* findHook(std::string name);
	static void removeHook(std::string name);
	
	static std::vector<unsigned char> getPrologue(void* functionAddress);
	static void relocateRelativeAddresses(void* oldLocation, void* newLocation, unsigned int length);
	static void* createCodeCave(void* functionAddress, std::vector<unsigned char> prologue);
	static void* installTrampoline(void* functionAddress, void* hookFunctionAddress, std::vector<unsigned char> prologue);
	static CinnamonResult installHWBP(void* functionAddress, HANDLE hookThread, HANDLE hThread = nullptr);
public:
	static CinnamonResult InstallHook(std::string name, void* functionAddress, void* hookFunctionAddress, void** originalFunction, HookType typeOfhook = HookType::ByteCodePatch);
	static CinnamonResult UninstallHook(std::string name);
	static void UninstallAllHooks();
};
