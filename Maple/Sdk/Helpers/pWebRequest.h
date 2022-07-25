#pragma once

#include "CLR/CLRString.h"

class pWebRequest
{
	typedef void(__fastcall* fnAddParameter)(uintptr_t instance, CLRString* name, CLRString* value);
	static inline fnAddParameter oAddParameter;
	static void __fastcall addParameterHook(uintptr_t instance, CLRString* name, CLRString* value);
public:
	static void Initialize();
};
