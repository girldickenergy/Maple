#pragma once

#include <COM/COMString.h>

class RichPresence
{
	typedef void(__fastcall* fnSetLargeImageText)(void* instance, COMString* string);
public:
	static inline fnSetLargeImageText oSetLargeImageText;
	static void __fastcall SetLargeImageTextHook(void* instance, COMString* string);
};
