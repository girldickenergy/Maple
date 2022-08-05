#pragma once

#include "CLR/CLRString.h"

class Obfuscated
{
	typedef CLRString* (__fastcall* fnGetStringValue)(uintptr_t instance);
	static inline fnGetStringValue oGetStringValue;
	static CLRString* __fastcall getStringValueHook(uintptr_t instance);

	typedef void(__fastcall* fnSetStringValue)(uintptr_t instance, CLRString* value);
	static inline fnSetStringValue oSetStringValue;
	static void __fastcall setStringValueHook(uintptr_t instance, CLRString* value);

	static inline constexpr int VALUE_OFFSET = 0x8;
	static inline constexpr int KEY_OFFSET = 0xC;
public:
	static void Initialize();

	static CLRString* GetString(uintptr_t instance);
	static void SetString(uintptr_t instance, CLRString* value);
	
	static int GetInt(uintptr_t instance);
	static void SetInt(uintptr_t instance, int value);
};
