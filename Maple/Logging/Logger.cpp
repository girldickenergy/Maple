#include "Logger.h"

#include <iomanip>
#include <iostream>
#include <sstream>
#include <fstream>

#include <VirtualizerSDK.h>

#include "xorstr.hpp"
#include "../Storage/Storage.h"
#include "../Utilities/Crypto/CryptoUtilities.h"

#include "Charlotte.h"

void Logger::clearLogFile()
{
	if (exists(logFilePath))
	{
		std::ifstream ifs(logFilePath);
		const std::string logData((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
		ifs.close();

		previousRuntimeLogData = logData;
	}

	if (logFilePath.empty())
		return;

	Storage::EnsureDirectoryExists(Storage::LogsDirectory);

	std::fstream logFile;
	logFile.open(logFilePath, std::ios_base::out | std::ios_base::trunc);
	logFile.close();

	if (performanceLogFilePath.empty())
		return;

	std::fstream performanceReportFile;
	performanceReportFile.open(performanceLogFilePath, std::ios_base::out | std::ios_base::trunc);
	performanceReportFile.close();
}

void Logger::createLogEntry(LogSeverity severity, const char* message)
{
	VIRTUALIZER_FISH_WHITE_START

	const auto time = std::time(nullptr);
	tm timeStruct{};
	localtime_s(&timeStruct, &time);

	char timeBuf[32];
	std::strftime(timeBuf, 32, "%d.%m.%Y %H:%M:%S", &timeStruct);

	EncryptedString entry = xorstr_("[") + EncryptedString(timeBuf) + xorstr_("] ");

	switch (severity)
	{
		case LogSeverity::Debug:
			if (consoleHandle)
				SetConsoleTextAttribute(consoleHandle, 8);

			entry += xorstr_("[DEBUG] ");
			break;
		case LogSeverity::Warning:
			if (consoleHandle)
				SetConsoleTextAttribute(consoleHandle, 6);

			entry += xorstr_("[WARNING] ");
			break;
		case LogSeverity::Error:
			if (consoleHandle)
				SetConsoleTextAttribute(consoleHandle, 4);

			entry += xorstr_("[ERROR] ");
			break;
		case LogSeverity::Assert:
			if (consoleHandle)
				SetConsoleTextAttribute(consoleHandle, 5);

			entry += xorstr_("[ASSERT FAIL] ");
			break;
		default:
			if (consoleHandle)
				SetConsoleTextAttribute(consoleHandle, 7);

			entry += xorstr_("[INFO] ");
			break;
	}

	entry += message;

	char entryBuf[entry.GetSize()];
	entry.GetData(entryBuf);

	if (consoleHandle)
		std::cout << entryBuf << std::endl;

	if (logFilePath.empty())
		return;

	Storage::EnsureDirectoryExists(Storage::LogsDirectory);

	std::fstream logFile;
	logFile.open(logFilePath, std::ios_base::out | std::ios_base::app);
#ifdef CLEAR_TEXT_LOGS
	logFile << entryBuf << std::endl;
#else
	logFile << CryptoUtilities::Base64Encode(CryptoUtilities::MapleXOR(entryBuf, entry.GetSize() - 1, xorstr_("hRaIpIYfKXDQS1CTLT7T4vAo"))) << std::endl;
#endif
	logFile.close();

	VIRTUALIZER_FISH_WHITE_END
}

void Logger::Initialize(LogSeverity scope, bool initializeConsole, LPCWSTR consoleTitle)
{
	VIRTUALIZER_FISH_RED_START

	auto& charlotte = Charlotte::Get();

	Logger::logFilePath = Storage::LogsDirectory + xorstr_("\\runtime.log");
	Logger::crashLogFilePath = Storage::LogsDirectory + xorstr_("\\crash.log");
	Logger::performanceLogFilePath = Storage::LogsDirectory + xorstr_("\\performance.log");
	Logger::scope = scope;

	if (initializeConsole)
	{
		charlotte.Call<BOOL>(reinterpret_cast<uintptr_t>(AllocConsole));
		charlotte.Call<errno_t>(reinterpret_cast<uintptr_t>(freopen_s), reinterpret_cast<FILE**>(stdout), xorstr_("CONOUT$"), xorstr_("w"), stdout);
		consoleHandle = charlotte.Call<HANDLE>(reinterpret_cast<uintptr_t>(GetStdHandle), STD_OUTPUT_HANDLE);

		SetConsoleTitle(consoleTitle);
	}

	clearLogFile();

	VIRTUALIZER_FISH_RED_END
}

void Logger::WriteCrashReport(const std::string& crashReport)
{
	std::fstream logFile;
	logFile.open(crashLogFilePath, std::ios_base::out | std::ios_base::trunc);
#ifdef CLEAR_TEXT_LOGS
	logFile << crashReport;
#else
	logFile << CryptoUtilities::Base64Encode(CryptoUtilities::MapleXOR(crashReport, xorstr_("hRaIpIYfKXDQS1CTLT7T4vAo")));
#endif
	logFile.close();
}

void Logger::Log(LogSeverity severity, const char* format, ...)
{
	if (static_cast<int>(severity & scope) > 0)
	{
		const int size = 1024;

		auto& charlotte = Charlotte::Get();

		char buffer[size];
		va_list args;
		__builtin_va_start(args, format);
		charlotte.Call<int>(reinterpret_cast<uintptr_t>(vsnprintf), buffer, size, format, args);
		__builtin_va_end(args);

		createLogEntry(severity, buffer);
	}
}

void Logger::Assert(bool result, bool throwIfFalse, const char* format, ...)
{
	if (!result)
	{
		const int size = 1024;
		auto& charlotte = Charlotte::Get();

		char buffer[size];
		va_list args;
		__builtin_va_start(args, format);
		charlotte.Call<int>(reinterpret_cast<uintptr_t>(vsnprintf), buffer, size, format, args);
		__builtin_va_end(args);

		if (static_cast<int>(LogSeverity::Assert & scope) > 0)
			createLogEntry(LogSeverity::Assert, buffer);

		if (throwIfFalse)
			throw std::runtime_error(std::string(xorstr_("Assertion failed: ")) + buffer);
	}
}

void Logger::StartPerformanceCounter(const std::string& guid)
{
	auto time = std::time(nullptr);
	tm timeStruct{};
	localtime_s(&timeStruct, &time);
	performanceLogMap.emplace(std::make_pair(guid, std::make_pair(timeStruct, timeStruct)));
}

void Logger::StopPerformanceCounter(const std::string& guid)
{
	VIRTUALIZER_MUTATE_ONLY_START

	std::pair<std::string, std::tuple<tm, tm>> performanceReport = {};
	// Find index
	for (auto& pr : performanceLogMap)
	{
		if (pr.first == guid)
		{
			performanceReport = pr;
			break;
		}
	}

	if (performanceReport.first.empty())
		return;

	auto time = std::time(nullptr);
	tm timeStruct{};
	localtime_s(&timeStruct, &time);
	performanceReport.second = std::make_pair(std::get<0>(performanceReport.second), timeStruct);

	// Write to file
	if (performanceLogFilePath.empty())
		return;

	char timeStartBuf[32];
	std::strftime(timeStartBuf, 32, "%d.%m.%Y %H:%M:%S", &std::get<0>(performanceReport.second));

	char timeEndBuf[32];
	std::strftime(timeEndBuf, 32, "%d.%m.%Y %H:%M:%S", &std::get<1>(performanceReport.second));

	EncryptedString entry = xorstr_("Performance Counter finished! ") + EncryptedString(guid.c_str()) + xorstr_(" Start: ") + EncryptedString(timeStartBuf) + xorstr_(" End: ") + EncryptedString(timeEndBuf);

	Storage::EnsureDirectoryExists(Storage::LogsDirectory);

	char entryBuf[entry.GetSize()];
    entry.GetData(entryBuf);

	std::fstream logFile;
	logFile.open(performanceLogFilePath, std::ios_base::out | std::ios_base::app);
#ifdef CLEAR_TEXT_LOGS
	logFile << entryBuf << std::endl;
#else
	logFile << CryptoUtilities::Base64Encode(CryptoUtilities::MapleXOR(entryBuf, entry.GetSize() - 1, xorstr_("hRaIpIYfKXDQS1CTLT7T4vAo"))) << std::endl;
#endif
	logFile.close();

	// Remove from performanceReportMap
	performanceLogMap.erase(guid);

	VIRTUALIZER_MUTATE_ONLY_END
}

std::string Logger::GetPreviousRuntimeLogData()
{
	return previousRuntimeLogData;
}

std::string Logger::GetRuntimeLogData()
{
	if (!exists(logFilePath))
		return {};

	std::ifstream ifs(logFilePath);
	std::string logData((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
	ifs.close();

	return logData;
}

std::string Logger::GetCrashLogData()
{
	if (!exists(crashLogFilePath))
		return {};

	std::ifstream ifs(crashLogFilePath);
	std::string crashData((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
	ifs.close();

	return crashData;
}

std::string Logger::GetPerformanceLogData()
{
	if (!exists(performanceLogFilePath))
		return {};

	std::ifstream ifs(performanceLogFilePath);
	std::string performanceData((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
	ifs.close();

	return performanceData;
}
