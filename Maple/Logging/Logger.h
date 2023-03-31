#pragma once

#include <filesystem>
#include <string>
#include <windows.h>

#include "LogSeverity.h"
#include <map>

class Logger
{
	static inline bool shouldEncrypt = false;
	static inline HANDLE consoleHandle = nullptr;
	static inline LogSeverity scope;
	static inline std::filesystem::path logFilePath;
	static inline std::filesystem::path crashReportFilePath;
	static inline std::filesystem::path performanceReportFilePath;
	static inline std::string previousRuntimeLogData;

	static inline std::map<std::string, std::tuple<tm, tm>> performanceReportMap;

	static void clearLogFile();
	static void createLogEntry(LogSeverity severity, std::string message);
public:
	static void Initialize(LogSeverity scope = LogSeverity::None, bool encrypt = false, bool initializeConsole = false, LPCWSTR consoleTitle = L"");
	static void WriteCrashReport(const std::string& crashReport);
	static void Log(LogSeverity severity, const char* format, ...);
	static void Assert(bool condition, bool throwIfFalse, const char* format, ...);
	static void StartPerformanceCounter(const std::string& guid);
	static void StopPerformanceCounter(const std::string& guid);
	static std::string GetPreviousRuntimeLogData();
	static std::string GetRuntimeLogData();
	static std::string GetCrashReportData();
};