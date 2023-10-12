#pragma once

#include <wtypes.h>
#include <string>

#include "LogLevel.h"
#include "LoggerInfo.h"

class Logger
{
    bool m_IsEnabled = true;
    std::string m_Name;
    LoggerInfo m_LoggerInfo;
    LogLevel m_LogLevel = LogLevel::Verbose;
    HANDLE m_ConsoleHandle = nullptr;

    std::string GetFileName();
    unsigned short LogLevelToConsoleColor(LogLevel logLevel);
    std::string LogLevelToString(LogLevel logLevel);
    void CreateInfoLogMessage();
    void CreateLogEntry(LogLevel logLevel, const std::string& message);

public:
    /**
     * \brief Creates a new Logger instance
     * \param name The name of this Logger
     * \param loggerInfo Maple, osu!, osu!auth and environment information of this Logger
     * \param logLevel The minimum log-level a logged message needs to have to be logged. Default is LogLevel::Verbose
     * \param consoleHandle The handle of a console window to log messages to. Default is null
     */
    Logger(const std::string& name, const LoggerInfo& loggerInfo, LogLevel logLevel = LogLevel::Verbose, HANDLE consoleHandle = nullptr);

    /**
     * \brief Creates a new log entry
     * \param logLevel The log-level of this message
     * \param format The log message
     * \param ... The log message parameters
     */
    void Log(LogLevel logLevel, const char* format, ...);
    /**
     * \brief Enables or disables this Logger
     * \param value Whether this Logger should be enabled or not
     */
    void ToggleEnabled(bool value);
    /**
     * \returns Whether or not this Logger is enabled
     */
    bool GetIsEnabled();
};
