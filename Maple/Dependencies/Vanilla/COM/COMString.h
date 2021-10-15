#pragma once

#include <string>

class COMString
{
	struct UnicodeString
	{
		int padding;
		int length;

		wchar_t* buffer;
	};

	UnicodeString ptr;

public:
	int GetLength()
	{
		return ptr.length;
	}

	std::wstring_view GetData()
	{
		return (const wchar_t*)&ptr.buffer;
	}
};
