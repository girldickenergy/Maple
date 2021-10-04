#pragma once

#include <filesystem>
#include <string>
#include <windows.h>

#include "LogSeverity.h"

class Logger
{
	HANDLE consoleHandle = nullptr;
	LogSeverity scope;
	std::filesystem::path logFilePath;

	void createLogEntry(LogSeverity severity, std::string message);
public:
	void Initialize(const char* logFilePath, LogSeverity scope = LogSeverity::Debug | LogSeverity::Warning | LogSeverity::Error | LogSeverity::Assert, bool initializeConsole = false, LPCWSTR consoleTitle = L"");
	void Log(LogSeverity severity, const char* format, ...);
	void Assert(bool condition, bool throwIfFalse, const char* format, ...);
};
