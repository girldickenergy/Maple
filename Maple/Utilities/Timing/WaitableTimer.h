#pragma once

#include <windows.h>

class WaitableTimer
{
	static inline HANDLE timer = CreateWaitableTimer(NULL, FALSE, NULL);
public:
	static void Sleep(unsigned relativeTime100Ns)
	{
		LARGE_INTEGER dueTime = { 0 };
		dueTime.QuadPart = static_cast<LONGLONG>(relativeTime100Ns) * -1;

		BOOL res = ::SetWaitableTimer(timer, &dueTime, 0, NULL, NULL, FALSE);
		DWORD waitRes = ::WaitForSingleObject(timer, INFINITE);
	}
};