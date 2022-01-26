#include <iostream>
#include <thread>
#include <windows.h>
#include <clocale>

#include <ThemidaSDK.h>

#include "Communication/Communication.h"
#include "Config/Config.h"
#include "Features/Timewarp/Timewarp.h"
#include "Hooks/Hooks.h"
#include "Sdk/Anticheat/Anticheat.h"
#include "Sdk/Audio/AudioEngine.h"
#include "Sdk/Bindings/BindingManager.h"
#include "Sdk/ConfigManager/ConfigManager.h"
#include "Sdk/Input/InputManager.h"
#include "Sdk/Mods/ModManager.h"
#include "Sdk/Osu/GameBase.h"
#include "Sdk/Player/HitObjectManager.h"
#include "Sdk/Player/Player.h"
#include "Sdk/Player/Ruleset.h"
#include "Utilities/Logging/Logger.h"
#include "Sdk/Osu/GameField.h"
#include "Utilities/Security/Security.h"
#include "Utilities/Strings/StringUtilities.h"
#include "Features/Spoofer/Spoofer.h"
#include "Utilities/Directories/DirectoryHelper.h"

#include <curl.h>
#include "Sdk/Osu/WindowManager.h"

DWORD WINAPI Initialize(LPVOID data_addr);
void InitializeMaple();
void InitializeLogging();
void InitializeSdk();
void StartFunctions();

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    std::setlocale(LC_NUMERIC, "en_US");
    DisableThreadLibraryCalls(hModule);

    if (ul_reason_for_call == DLL_PROCESS_ATTACH)
        CreateThread(nullptr, 0, Initialize, lpReserved, 0, nullptr);
	
    return TRUE;
}

struct ArgsBase
{
    long long size;
};

struct CustomArgs : ArgsBase
{
    char user_data[256 * 5 * 10];
};

DWORD WINAPI Initialize(LPVOID data_addr)
{
    VM_SHARK_BLACK_START
    STR_ENCRYPT_START
    auto pArgs = (CustomArgs*)data_addr;

    std::string data(pArgs->user_data, 255);

    std::vector<std::string> split = StringUtilities::Split(data);

    Communication::CurrentUser = new User(split[0], split[1], split[2], split[3]);

    Communication::ConnectToServer();

    curl_global_init(CURL_GLOBAL_ALL);

    while (!Communication::EstablishedConnection || !Communication::HeartbeatThreadLaunched || !Communication::HandshakeSucceeded)
        Sleep(500);

    InitializeMaple();
    STR_ENCRYPT_END
    VM_SHARK_BLACK_END

    return 0;
}

void InitializeMaple()
{
    if (!Communication::EstablishedConnection || !Communication::HeartbeatThreadLaunched || !Communication::HandshakeSucceeded)
        Security::CorruptMemory();

    DirectoryHelper::Initialize();
	
    Vanilla::Initialize();

    InitializeLogging();

    InitializeSdk();

    Config::Initialize();

    Hooks::InstallAllHooks();

    StartFunctions();
}

void InitializeLogging()
{
    VM_FISH_RED_START
    STR_ENCRYPT_START
	
    if (!Communication::EstablishedConnection || !Communication::HeartbeatThreadLaunched || !Communication::HandshakeSucceeded)
        Security::CorruptMemory();
	
#ifdef _DEBUG
    Logger::Initialize(LogSeverity::All, true, L"Runtime log | Maple");
#else
    Logger::Initialize(LogSeverity::Error | LogSeverity::Debug | LogSeverity::Assert | LogSeverity::Warning);
#endif
	
    Logger::Log(LogSeverity::Info, xor ("Initialization started."));

    VM_FISH_RED_END
    STR_ENCRYPT_END
}

void InitializeSdk()
{
    if (!Communication::EstablishedConnection || !Communication::HeartbeatThreadLaunched || !Communication::HandshakeSucceeded)
        Security::CorruptMemory();
	
    Anticheat::Initialize();
    GameBase::Initialize();
    GameField::Initialize();
    WindowManager::Initialize();
    InputManager::Initialize();
    ConfigManager::Initialize();
    BindingManager::Initialize();
    AudioEngine::Initialize();
    ModManager::Initialize();
    Player::Initialize();
    Ruleset::Initialize();
    HitObjectManager::Initialize();
}

void StartFunctions()
{
    VM_FISH_RED_START
	
    if (!Communication::EstablishedConnection || !Communication::HeartbeatThreadLaunched || !Communication::HandshakeSucceeded)
        Security::CorruptMemory();
	
    Anticheat::DisableAnticheat();
    Timewarp::Initialize();
    Spoofer::Initialize();

    VM_FISH_RED_END
}
