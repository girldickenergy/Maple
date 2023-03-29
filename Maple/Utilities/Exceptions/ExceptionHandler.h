#pragma once

#include <windows.h>

class ExceptionHandler
{
	static LONG __stdcall pVectoredExceptionHandler(EXCEPTION_POINTERS* ExceptionInfo);
public:
	static void Setup();
};