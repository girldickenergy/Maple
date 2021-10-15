#pragma once

#include <map>

#include "../../Vanilla.h"

class FieldHelpers
{
	typedef void* (__thiscall* fnGetCurrentStaticAddress)(void* pFieldDesc);
	static inline fnGetCurrentStaticAddress getCurrentStaticAddress = reinterpret_cast<fnGetCurrentStaticAddress>(Vanilla::FindSignature("\x56\x8B\xF1\xF7\x46\x00\x00\x00\x00\x00\x0F\x85\x00\x00\x00\x00\xF7", "xxxxx?????xx????x", (uintptr_t)GetModuleHandleA("clr.dll"), Vanilla::GetModuleSize("clr.dll")));

	static inline std::map<void*, void*> addressLookup;
public:
	static void* GetStaticFieldAddress(void* pFieldDesc)
	{
		void* address = addressLookup[pFieldDesc];
		if (!address)
			address = getCurrentStaticAddress(pFieldDesc);
		
		return address;
	}
};
