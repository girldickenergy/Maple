#pragma once

#include <string>

#include <Vanilla.h>

class COMString
{
	typedef COMString*(__cdecl* fnNewString)(const wchar_t* pwsz);
	static inline fnNewString newString = reinterpret_cast<fnNewString>(Vanilla::FindSignature("\x53\x8B\xD9\x56\x57\x85\xDB\x0F", "xxxxxxxx", reinterpret_cast<uintptr_t>(GetModuleHandleA("clr.dll")), Vanilla::GetModuleSize("clr.dll")));

	struct StringObject
	{
		void* vtable;
		int length;
		wchar_t* buffer;
	};

	StringObject ptr;
public:
	static COMString* CreateString(const wchar_t* pwsz)
	{
		return newString(pwsz);
	}
	
	int Length() const
	{
		return ptr.length;
	}
	
	std::wstring_view Data() const
	{
		return reinterpret_cast<const wchar_t*>(&ptr.buffer);
	}
};
