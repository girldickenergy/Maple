#include <iostream>
#include <thread>
#include <windows.h>
#include <clocale>

#include "Features/Timewarp/Timewarp.h"
#include "Features/Visuals/VisualsSpoofers.h"
#include "Hooks/Hooks.h"
#include "Sdk/Anticheat/Anticheat.h"
#include "Sdk/Audio/AudioEngine.h"
#include "Sdk/ConfigManager/ConfigManager.h"
#include "Sdk/Mods/ModManager.h"
#include "Sdk/Osu/GameBase.h"
#include "Sdk/Player/HitObjectManager.h"
#include "Sdk/Player/Player.h"
#include "Sdk/Player/Ruleset.h"
#include "Utilities/Logging/Logger.h"

DWORD WINAPI Initialize(LPVOID data_addr);
void InitializeMaple(const std::string& username);
void InitializeLogging(const std::string& username);
void InitializeSdk();
void StartFunctions();

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

    InitializeLogging(username);

    InitializeSdk();

    Hooks::InstallAllHooks();

    StartFunctions();
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

void InitializeSdk()
{
    Player::Initialize();
    AudioEngine::Initialize();
    GameBase::Initialize();
    Anticheat::Initialize();
    ConfigManager::Initialize();
    ModManager::Initialize();
    Ruleset::Initialize();
    HitObjectManager::Initialize();
}

void StartFunctions()
{
    Anticheat::DisableAnticheat();
	
    CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(Timewarp::TimewarpThread), nullptr, 0, nullptr);
    CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(VisualsSpoofers::FlashlightRemoverThread), nullptr, 0, nullptr);
}
