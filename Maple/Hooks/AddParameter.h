#pragma once

#include <COM/COMString.h>

class AddParameter
{
	typedef void(__fastcall* fnAddParameter)(void* instance, COMString* name, COMString* value);
public:
	static inline fnAddParameter oAddParameter;
	static void __fastcall AddParameterHook(void* instance, COMString* name, COMString* value);
};