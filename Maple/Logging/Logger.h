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
	static inline std::string previousRuntimeLogData;

	static void clearLogFile();
	static void createLogEntry(LogSeverity severity, const char* message);
public:
	static void Initialize(LogSeverity scope = LogSeverity::None, bool initializeConsole = false, LPCWSTR consoleTitle = L"");
	static void Log(LogSeverity severity, const char* format, ...);
	static std::string GetPreviousRuntimeLogData();
	static std::string GetRuntimeLogData();
};