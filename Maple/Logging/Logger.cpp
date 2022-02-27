#include "Logger.h"

#include <iomanip>
#include <iostream>
#include <sstream>
#include <fstream>

#include "../Config/Config.h"
#include "../Storage/Storage.h"

void Logger::clearLogFile()
{
	if (logFilePath.empty())
		return;

	Storage::EnsureDirectoryExists(Storage::LogsDirectory);

	std::fstream logFile;
	logFile.open(logFilePath, std::ios_base::out | std::ios_base::trunc);
	logFile.close();
}

void Logger::createLogEntry(LogSeverity severity, std::string message)
{
	if (Config::Misc::DisableLogging)
		return;
	
	std::ostringstream entry;

	auto time = std::time(nullptr);
	tm timeStruct{};
	localtime_s(&timeStruct, &time);

	entry << "[" << std::put_time(&timeStruct, "%c") << "] ";

	switch (severity)
	{
	case LogSeverity::Debug:
		if (consoleHandle)
			SetConsoleTextAttribute(consoleHandle, 8);

		entry << "[DEBUG] ";
		break;
	case LogSeverity::Warning:
		if (consoleHandle)
			SetConsoleTextAttribute(consoleHandle, 6);

		entry << "[WARNING] ";
		break;
	case LogSeverity::Error:
		if (consoleHandle)
			SetConsoleTextAttribute(consoleHandle, 4);

		entry << "[ERROR] ";
		break;
	case LogSeverity::Assert:
		if (consoleHandle)
			SetConsoleTextAttribute(consoleHandle, 5);

		entry << "[ASSERT FAIL] ";
		break;
	default:
		if (consoleHandle)
			SetConsoleTextAttribute(consoleHandle, 7);

		entry << "[INFO] ";
		break;
	}

	entry << message;

	if (consoleHandle)
		std::cout << entry.str() << std::endl;

	if (logFilePath.empty())
		return;

	Storage::EnsureDirectoryExists(Storage::LogsDirectory);

	std::fstream logFile;
	logFile.open(logFilePath, std::ios_base::out | std::ios_base::app);
	logFile << entry.str() << std::endl;
	logFile.close();
}

void Logger::Initialize(LogSeverity scope, bool initializeConsole, LPCWSTR consoleTitle)
{
	Logger::logFilePath = Storage::LogsDirectory + "\\runtime.log";
	Logger::scope = scope;
	
	if (initializeConsole)
	{
		AllocConsole();
		freopen_s(reinterpret_cast<FILE**>(stdout), "CONOUT$", "w", stdout);
		consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);

		SetConsoleTitle(consoleTitle);
	}

	clearLogFile();
}

void Logger::Log(LogSeverity severity, const char* format, ...)
{
	if (static_cast<int>(severity & scope) > 0)
	{
		char buffer[1024];
		va_list args;
		va_start(args, format);
		vsprintf_s(buffer, format, args);
		va_end(args);

		createLogEntry(severity, std::string(buffer));
	}
}

void Logger::Assert(bool result, bool throwIfFalse, const char* format, ...)
{
	if (!result)
	{
		char buffer[1024];
		va_list args;
		va_start(args, format);
		vsprintf_s(buffer, format, args);
		va_end(args);
		
		if (static_cast<int>(LogSeverity::Assert & scope) > 0)
			createLogEntry(LogSeverity::Assert, std::string(buffer));

		if (throwIfFalse)
			throw std::runtime_error(std::string("Assertion failed: ") + buffer);
	}
}
