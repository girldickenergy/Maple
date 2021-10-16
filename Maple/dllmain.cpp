#include <iostream>
#include <thread>
#include <windows.h>
#include <clocale>

#include <Vanilla.h>

#include "Hooks/Hooks.h"
#include "Sdk/ConfigManager/ConfigManager.h"
#include "Utilities/Logging/Logger.h"

DWORD WINAPI Initialize(LPVOID data_addr);
void InitializeMaple(const std::string& username);
void InitializeLogging(const std::string& username);
void DisableAuth();
void InitializeSdk();

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

    return 0;
}

void InitializeMaple(const std::string& username)
{
    Vanilla::Initialize();

    InitializeSdk();
	
    InitializeLogging(username);

#ifdef _DEBUG
    DisableAuth();
#endif

    Hooks::InstallAllHooks();
}

void InitializeLogging(const std::string& username)
{
#ifdef _DEBUG
    Logger::Initialize(GetWorkingDirectory(username) + "\\runtime.log", LogSeverity::All, true, L"Runtime log | Maple");
#else
    Logger::Initialize(GetWorkingDirectory(username) + "\\runtime.log", LogSeverity::All);
#endif
	
    Logger::Log(LogSeverity::Info, "Initialization started.");
}

void DisableAuth()
{
    auto instance = Vanilla::Explorer["osu.GameBase"]["Instance"].Field.GetValueUnsafe(variant_t());
    Vanilla::Explorer["osu.GameBase"]["FreeAC"].Method.InvokeUnsafe(instance, nullptr);
}

void InitializeSdk()
{
    ConfigManager::Initialize();
}
