#pragma once

#include <filesystem>
#include <string>
#include <windows.h>

#include "LogSeverity.h"

class Logger
{
	static inline bool shouldEncrypt = false;
	static inline HANDLE consoleHandle = nullptr;
	static inline LogSeverity scope;
	static inline std::filesystem::path logFilePath;

	static void clearLogFile();
	static void createLogEntry(LogSeverity severity, std::string message);
public:
	static void Initialize(LogSeverity scope = LogSeverity::None, bool encrypt = false, bool initializeConsole = false, LPCWSTR consoleTitle = L"");
	static void Log(LogSeverity severity, const char* format, ...);
	static void Assert(bool condition, bool throwIfFalse, const char* format, ...);
};