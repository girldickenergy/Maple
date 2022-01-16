#pragma once

#include <Vanilla.h>
#include <Hook.h>
#include <CinnamonResult.h>

class Hooks
{
	static CinnamonResult installManagedHook(std::string name, Method method, LPVOID pDetour, LPVOID* ppOriginal, HookType hookType = HookType::ByteCodePatch);
public:
	static void InstallAllHooks();
	static void UninstallAllHooks();
};
