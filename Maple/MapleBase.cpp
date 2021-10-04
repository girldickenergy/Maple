#include "MapleBase.h"

void MapleBase::Initialize()
{
	Logger = new ::Logger();
	Logger->Initialize("Maple\\logs\\runtime.log",  LogSeverity::Debug | LogSeverity::Warning | LogSeverity::Error | LogSeverity::Assert, true, L"Runtime log | Maple");
	Logger->Log(LogSeverity::Info, "Info severity test.");
	Logger->Log(LogSeverity::Debug, "Debug severity test.");
	Logger->Log(LogSeverity::Warning, "Warning severity test.");
	Logger->Log(LogSeverity::Error, "Error severity test.");
	Logger->Log(LogSeverity::Info, "Info severity test with parameter: %i.", 1337);
	Logger->Log(LogSeverity::Debug, "Debug severity test with parameter: %i.", 1337);
	Logger->Log(LogSeverity::Warning, "Warning severity test with parameter: %i.", 1337);
	Logger->Log(LogSeverity::Error, "Error severity test with parameter: %i.", 1337);
	Logger->Assert(1 == 1, true, "Assertion test (1 == 1, exitIfFalse == true)");
	Logger->Assert(1 == 0, false, "Assertion test (1 == 0, exitIfFalse == false, %i)", 1488);
}
