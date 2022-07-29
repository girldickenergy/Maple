#pragma once

#include "CLR/CLRString.h"

class ObfuscatedString
{
	typedef CLRString* (__fastcall* fnGetValue)(uintptr_t instance);
	static inline fnGetValue oGetValue;
	static CLRString* __fastcall getValueHook(uintptr_t instance);

	typedef void(__fastcall* fnSetValue)(uintptr_t instance, CLRString* value);
	static inline fnSetValue oSetValue;
	static void __fastcall setValueHook(uintptr_t instance, CLRString* value);
public:
	static void Initialize();

	static CLRString* GetValue(uintptr_t instance);
	static void SetValue(uintptr_t instance, CLRString* value);
};
