#include "MapleBase.h"

void MapleBase::Initialize()
{
	Logger = new ::Logger();
	Logger->Initialize("Maple\\logs\\runtime.log", LogSeverity::Info | LogSeverity::Debug | LogSeverity::Warning | LogSeverity::Error | LogSeverity::Assert, true, L"Runtime log | Maple");
	Logger->Log(LogSeverity::Info, "Initialization started.");
}
