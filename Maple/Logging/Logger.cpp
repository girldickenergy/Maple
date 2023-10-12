#include "Logger.h"

#include <iomanip>
#include <iostream>
#include <sstream>
#include <fstream>

#include "xorstr.h"

std::string Logger::GetFileName()
{
    return m_Name + xorstr_(".log");
}

unsigned short Logger::LogLevelToConsoleColor(LogLevel logLevel)
{
    switch (logLevel)
    {
        case LogLevel::Debug:
            return 8;
        case LogLevel::Verbose:
            return 7;
        case LogLevel::Important:
            return 4;
        case LogLevel::Error:
            return 5;
    }

    return 7;
}

std::string Logger::LogLevelToString(LogLevel logLevel)
{
    switch (logLevel)
    {
        case LogLevel::Debug:
            return xorstr_("Debug");
        case LogLevel::Verbose:
            return xorstr_("Verbose");
        case LogLevel::Important:
            return xorstr_("Important");
        case LogLevel::Error:
            return xorstr_("Error");
    }

    return xorstr_("Verbose");
}

void Logger::CreateInfoLogMessage()
{
    Log(LogLevel::Verbose, xorstr_("----------------------------------------------------------"));
    Log(LogLevel::Verbose, xorstr_("%s log (LogLevel: %s)"), m_Name.c_str(), LogLevelToString(m_LogLevel).c_str());
    Log(LogLevel::Verbose, xorstr_("Running %s on CLR %s"), m_LoggerInfo.MapleVersion.c_str(), m_LoggerInfo.CLRVersion.c_str());
    Log(LogLevel::Verbose, xorstr_("osu! hash: %s, osu!auth hash: %s"), m_LoggerInfo.OsuHash.c_str(), m_LoggerInfo.OsuAuthHash.c_str());
    Log(LogLevel::Verbose, xorstr_("Environment: %s"), m_LoggerInfo.WindowsVersion.c_str());
    Log(LogLevel::Verbose, xorstr_("----------------------------------------------------------"));
}

void Logger::CreateLogEntry(LogLevel logLevel, const std::string& message)
{
    if (!m_IsEnabled)
        return;

    std::ostringstream entry;

    const auto time = std::time(nullptr);
    tm timeStruct{};
    localtime_s(&timeStruct, &time);

    entry << "[" << std::put_time(&timeStruct, "%c") << "] [" << LogLevelToString(logLevel) << "]: " << message;

    if (m_ConsoleHandle)
    {
        SetConsoleTextAttribute(m_ConsoleHandle, LogLevelToConsoleColor(logLevel));

        std::cout << entry.str() << std::endl;
    }
}

Logger::Logger(const std::string& name, const LoggerInfo& loggerInfo, LogLevel logLevel, HANDLE consoleHandle)
{
    m_Name = name;
    m_LoggerInfo = loggerInfo;
    m_LogLevel = logLevel;
    m_ConsoleHandle = consoleHandle;

    CreateInfoLogMessage();
}

void Logger::Log(LogLevel logLevel, const char* format, ...)
{
    if (!m_IsEnabled)
        return;

    if (logLevel >= m_LogLevel)
    {
        char buffer[1024];
        va_list args;
        va_start(args, format);
        vsprintf_s(buffer, format, args);
        va_end(args);
        CreateLogEntry(logLevel, buffer);
    }
}

void Logger::ToggleEnabled(bool value)
{
    m_IsEnabled = value;
}

bool Logger::GetIsEnabled()
{
    return m_IsEnabled;
}
