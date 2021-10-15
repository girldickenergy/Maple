#include <iostream>
#include <thread>
#include <windows.h>
#include <clocale>

#include "Dependencies/Vanilla/Vanilla.h"
#include "Utilities/Logging/Logger.h"

DWORD WINAPI Initialize(LPVOID data_addr);
void InitializeMaple(const std::string& username);
void InitializeLogging(const std::string& username);
void DisableAuth();

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    std::setlocale(LC_NUMERIC, "en_US");
    DisableThreadLibraryCalls(hModule);

    if (ul_reason_for_call == DLL_PROCESS_ATTACH)
        CreateThread(nullptr, 0, Initialize, nullptr, 0, nullptr);
	
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

DWORD WINAPI Initialize(LPVOID data_addr)
{
	//initialize comms stuff here
    InitializeMaple("MapleRewriteTest");
}

void InitializeMaple(const std::string& username)
{
    Vanilla::Initialize();
	
    InitializeLogging(username);

#ifdef _DEBUG
    DisableAuth();
#endif
}

void InitializeLogging(const std::string& username)
{
    Logger::Initialize(GetWorkingDirectory(username) + "\\runtime.log", LogSeverity::All, true, L"Runtime log | Maple");
    Logger::Log(LogSeverity::Info, "Initialization started.");
}

void DisableAuth()
{
	
}
