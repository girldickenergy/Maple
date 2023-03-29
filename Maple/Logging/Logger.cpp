#include "Logger.h"

#include <iomanip>
#include <iostream>
#include <sstream>
#include <fstream>

#include <VirtualizerSDK.h>

#include "xorstr.hpp"
#include "../Storage/Storage.h"
#include "../Utilities/Crypto/CryptoUtilities.h"

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
}

void Logger::createLogEntry(LogSeverity severity, std::string message)
{
	std::ostringstream entry;

	auto time = std::time(nullptr);
	tm timeStruct{};
	localtime_s(&timeStruct, &time);

	entry << xorstr_("[") << std::put_time(&timeStruct, xorstr_("%c")) << xorstr_("] ");

	switch (severity)
	{
		case LogSeverity::Debug:
			if (consoleHandle)
				SetConsoleTextAttribute(consoleHandle, 8);

			entry << xorstr_("[DEBUG] ");
			break;
		case LogSeverity::Warning:
			if (consoleHandle)
				SetConsoleTextAttribute(consoleHandle, 6);

			entry << xorstr_("[WARNING] ");
			break;
		case LogSeverity::Error:
			if (consoleHandle)
				SetConsoleTextAttribute(consoleHandle, 4);

			entry << xorstr_("[ERROR] ");
			break;
		case LogSeverity::Assert:
			if (consoleHandle)
				SetConsoleTextAttribute(consoleHandle, 5);

			entry << xorstr_("[ASSERT FAIL] ");
			break;
		default:
			if (consoleHandle)
				SetConsoleTextAttribute(consoleHandle, 7);

			entry << xorstr_("[INFO] ");
			break;
	}

	entry << message;

	if (consoleHandle)
		std::cout << (shouldEncrypt ? CryptoUtilities::MapleXOR(entry.str(), xorstr_("vD5KJvfDRKZEaR9I")) : entry.str()) << std::endl;

	if (logFilePath.empty())
		return;

	Storage::EnsureDirectoryExists(Storage::LogsDirectory);

	std::fstream logFile;
	logFile.open(logFilePath, std::ios_base::out | std::ios_base::app);
	logFile << (shouldEncrypt ? CryptoUtilities::Base64Encode(CryptoUtilities::MapleXOR(entry.str(), xorstr_("vD5KJvfDRKZEaR9I"))) : entry.str()) << std::endl;
	logFile.close();
}

void Logger::Initialize(LogSeverity scope, bool encrypt, bool initializeConsole, LPCWSTR consoleTitle)
{
	VIRTUALIZER_FISH_RED_START

	shouldEncrypt = encrypt;

	Logger::logFilePath = Storage::LogsDirectory + xorstr_("\\runtime.log");
	Logger::crashReportFilePath = Storage::LogsDirectory + xorstr_("\\crashreport.log");
	Logger::scope = scope;

	if (initializeConsole)
	{
		AllocConsole();
		freopen_s(reinterpret_cast<FILE**>(stdout), xorstr_("CONOUT$"), xorstr_("w"), stdout);
		consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);

		SetConsoleTitle(consoleTitle);
	}

	clearLogFile();

	VIRTUALIZER_FISH_RED_END
}

void Logger::WriteCrashReport(std::string crashReport)
{
	std::fstream cr;
	cr.open(crashReportFilePath, std::ios_base::out | std::ios_base::trunc);
	cr.close();

	std::fstream logFile;
	logFile.open(crashReportFilePath, std::ios_base::out | std::ios_base::app);
	logFile << CryptoUtilities::Base64Encode(CryptoUtilities::MapleXOR(crashReport.c_str(), xorstr_("vD5KJvfDRKZEaR9I")));
	logFile.close();
}

void Logger::Log(LogSeverity severity, const char* format, ...)
{
	if (static_cast<int>(severity & scope) > 0)
	{
		char buffer[1024];
		va_list args;
		__builtin_va_start(args, format);
		vsprintf_s(buffer, format, args);
		__builtin_va_end(args);

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
			throw std::runtime_error(std::string(xorstr_("Assertion failed: ")) + buffer);
	}
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

std::string Logger::GetCrashReportData()
{
	if (!exists(crashReportFilePath))
		return {};

	std::ifstream ifs(crashReportFilePath);
	std::string crashData((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
	ifs.close();

	return crashData;
}
