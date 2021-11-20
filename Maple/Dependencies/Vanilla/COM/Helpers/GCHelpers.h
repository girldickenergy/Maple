#pragma once

#include <Vanilla.h>

class GCHelpers
{
	typedef int(__fastcall* fnSetGCLatencyMode)(int mode);
	typedef int(__fastcall* fnGetGCLatencyMode)();
	static inline fnSetGCLatencyMode setGCLatencyMode = reinterpret_cast<fnSetGCLatencyMode>(Vanilla::FindSignature("\x56\x57\x8B\x3D\x00\x00\x00\x00\x51\x8B\x07\x8B\x70\x48", "xxxx????xxxxxx", (uintptr_t)GetModuleHandleA("clr.dll"), Vanilla::GetModuleSize("clr.dll")));
	static inline fnGetGCLatencyMode getGCLatencyMode = reinterpret_cast<fnGetGCLatencyMode>(Vanilla::FindSignature("\x56\x57\x8B\x3D\x00\x00\x00\x00\x8B\x07\x8B\x70\x44", "xxxx????xxxxx", (uintptr_t)GetModuleHandleA("clr.dll"), Vanilla::GetModuleSize("clr.dll")));
public:
	static int SetGCLatencyMode(int mode)
	{
		return setGCLatencyMode(mode);
	}

	static int GetGCLatencyMode()
	{
		return getGCLatencyMode();
	}
};
