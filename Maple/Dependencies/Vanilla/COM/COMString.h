#pragma once

#include <string>

struct COMString
{
	void* vtable;
	int length;
	wchar_t buffer[512];
	
	std::wstring_view GetData()
	{
		return (const wchar_t*)&buffer;
	}
};
