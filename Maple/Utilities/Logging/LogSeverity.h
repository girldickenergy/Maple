#pragma once

enum class LogSeverity
{
	None = 0,
	Info = 1 << 0,
	Debug = 1 << 1,
	Warning = 1 << 2,
	Error = 1 << 3,
	Assert = 1 << 4,
	All = Info | Debug | Warning | Error | Assert
};

inline LogSeverity operator|(LogSeverity a, LogSeverity b)
{
	return static_cast<LogSeverity>(static_cast<int>(a) | static_cast<int>(b));
}

inline LogSeverity operator&(LogSeverity a, LogSeverity b)
{
	return static_cast<LogSeverity>(static_cast<int>(a) & static_cast<int>(b));
}
