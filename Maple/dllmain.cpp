#include <iostream>
#include <thread>
#include <windows.h>
#include <clocale>

#include "Utilities/Logging/Logger.h"

void Initialize();

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    std::setlocale(LC_NUMERIC, "en_US");
    DisableThreadLibraryCalls(hModule);

    if (ul_reason_for_call == DLL_PROCESS_ATTACH)
        CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(Initialize), nullptr, 0, nullptr);
	
    return TRUE;
}

std::string GetWorkingDirectory(const std::string& username)
{
    char* val;
    size_t len;
    errno_t err = _dupenv_s(&val, &len, "APPDATA"); //xor later

    std::string path(val);
    path += "\\" + username; //hash later
    
    return path;
}

void Initialize()
{
    Logger::Initialize(GetWorkingDirectory("MapleRewriteTest") + "\\runtime.log", LogSeverity::Info | LogSeverity::Debug | LogSeverity::Warning | LogSeverity::Error | LogSeverity::Assert, true, L"Runtime log | Maple");
    Logger::Log(LogSeverity::Info, "Initialization started.");
}
