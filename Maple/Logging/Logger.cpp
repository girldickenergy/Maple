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
		default:
			if (consoleHandle)
				SetConsoleTextAttribute(consoleHandle, 7);

			entry += xorstr_("[INFO] ");
			break;
	}

	entry += message;

	char entryBuf[entry.GetSize()];
	entry.GetData(entryBuf);

#if defined(CLEAR_TEXT_LOGS) || defined(_DEBUG)
	if (consoleHandle)
		std::cout << entryBuf << std::endl;
#endif

	if (logFilePath.empty())
		return;

	Storage::EnsureDirectoryExists(Storage::LogsDirectory);

	std::fstream logFile;
	logFile.open(logFilePath, std::ios_base::out | std::ios_base::app);
#ifdef CLEAR_TEXT_LOGS
	logFile << entryBuf << std::endl;
#else
	logFile << CryptoUtilities::Base64Encode(CryptoUtilities::EncryptLogEntry(entryBuf, entry.GetSize() - 1)) << "\n";
#endif
	logFile.close();

	VIRTUALIZER_FISH_WHITE_END
}

void Logger::Initialize(LogSeverity scope, bool initializeConsole, LPCWSTR consoleTitle)
{
	VIRTUALIZER_FISH_RED_START

	Logger::logFilePath = Storage::LogsDirectory + xorstr_("\\runtime.log");
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

void Logger::Log(LogSeverity severity, const char* format, ...)
{
	if (static_cast<int>(severity & scope) > 0)
	{
		const int size = 1024;

		char buffer[size];
		va_list args;
		__builtin_va_start(args, format);
		vsnprintf(buffer, size, format, args);
		__builtin_va_end(args);

		createLogEntry(severity, buffer);
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
