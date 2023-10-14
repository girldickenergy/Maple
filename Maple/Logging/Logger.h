#pragma once

#include <wtypes.h>
#include <string>

#include "LogLevel.h"
#include "LoggerInfo.h"
#include "../Storage/Storage.h"

class Logger
{
    bool m_IsEnabled = true;
    std::string m_PreviousLog;
    std::shared_ptr<Storage> m_Storage;
    std::string m_Name;
    LoggerInfo m_LoggerInfo;
    LogLevel m_LogLevel = LogLevel::Verbose;
    bool m_Encrypt = false;
    HANDLE m_ConsoleHandle = nullptr;

    std::string GetFileName();
    std::string GetFilePath();
    unsigned short LogLevelToConsoleColor(LogLevel logLevel);
    std::string LogLevelToString(LogLevel logLevel);
    void CreateInfoLogMessage();
    void CreateLogEntry(LogLevel logLevel, const std::string& message);

public:
    /**
     * \brief Creates a new Logger instance
     * \param storage The Storage to place logs inside
     * \param name The name of this Logger
     * \param loggerInfo Maple, osu!, osu!auth and environment information of this Logger
     * \param logLevel The minimum log-level a logged message needs to have to be logged. Default is LogLevel::Verbose
     * \param encrypt Whether or not this log should be encrypted on disk
     * \param consoleHandle The handle of a console window to log messages to. Default is null
     */
    Logger(const std::shared_ptr<Storage>& storage, const std::string& name, const LoggerInfo& loggerInfo, LogLevel logLevel = LogLevel::Verbose, bool encrypt = false, HANDLE consoleHandle = nullptr);

    /**
     * \brief Creates a new log entry
     * \param logLevel The log-level of this message
     * \param format The log message
     * \param ... The log message parameters
     */
    void Log(LogLevel logLevel, const char* format, ...);
    /**
     * \returns Returns the log from the previous session
     */
    std::string GetPreviousLog();
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
