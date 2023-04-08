#include <clocale>
#include <WinSock2.h>

#include "curl.h"
#include "VirtualizerSDK.h"
#include "Vanilla.h"
#include "xorstr.hpp"
#include "Communication/Communication.h"
#include "Hooking/VanillaHooking.h"

#include "Logging/Logger.h"
#include "Storage/Storage.h"
#include "Config/Config.h"
#include "Dependencies/Milk/Milk.h"

#include "Utilities/Security/Security.h"

#include "SDK/Memory.h"
#include "SDK/Audio/AudioEngine.h"
#include "SDK/Discord/DiscordRPC.h"
#include "SDK/GL/GLControl.h"
#include "SDK/Helpers/ErrorSubmission.h"
#include "SDK/Helpers/Obfuscated.h"
#include "SDK/Input/InputManager.h"
#include "SDK/Mods/ModManager.h"
#include "SDK/Osu/GameBase.h"
#include "SDK/Osu/GameField.h"
#include "SDK/Beatmaps/BeatmapManager.h"
#include "SDK/Player/HitObjectManager.h"
#include "SDK/Player/Player.h"
#include "SDK/Player/Ruleset.h"
#include "SDK/Scoring/Score.h"
#include "SDK/Streaming/StreamingManager.h"
#include "UI/UI.h"
#include "Utilities/Anticheat/AnticheatUtilities.h"
#include "Utilities/Strings/StringUtilities.h"
#include "Dependencies/Milk/MilkThread.h"
#include "Utilities/Exceptions/ExceptionHandler.h"

#include "Features/ReplayEditor/Editor.h"

DWORD WINAPI Initialize();
void InitializeMaple();
void WaitForCriticalSDKToInitialize();

static inline LPVOID data;
static inline MilkThread* initializeThread = nullptr;

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    std::setlocale(LC_NUMERIC, "en_US");
    DisableThreadLibraryCalls(hModule);

    if (ul_reason_for_call == DLL_PROCESS_ATTACH)
    {
        data = lpReserved;
        initializeThread = new MilkThread(reinterpret_cast<uintptr_t>(Initialize));
    }

    return TRUE;
}

struct UserData
{
    char Username[25];
    char SessionToken[33];
    char DiscordID[33];
    char DiscordAvatarHash[33];
};

DWORD WINAPI Initialize()
{
    initializeThread->CleanCodeCave();
    delete initializeThread;

    auto data_addr = data;

    VIRTUALIZER_TIGER_LITE_START
        Logger::StartPerformanceCounter(xorstr_("{6907431E-A1F9-4E21-BD08-8CF5078CB8D1}"));
    ExceptionHandler::Setup();

    if (!data_addr)
        Security::CorruptMemory();

    UserData userData = *static_cast<UserData*>(data_addr);
    Communication::SetUser(new User(userData.Username, userData.SessionToken, userData.DiscordID, userData.DiscordAvatarHash));

    // Initialize this a bit earlier just so we can log more data earlier.
    Storage::Initialize(Communication::GetUser()->GetUsernameHashed());

#ifdef _DEBUG
    Logger::Initialize(LogSeverity::All, false, true, L"Runtime log | Maple");
#else
    Logger::Initialize(LogSeverity::All, true);
#endif

    memset(data_addr, 0x0, sizeof(UserData));

    Communication::Connect();

    Logger::StartPerformanceCounter(xorstr_("{D7310D1B-17C9-42D2-9511-29906528545E}"));
    while (!Communication::GetIsConnected() || !Communication::GetIsHandshakeSucceeded() || !Communication::GetIsHeartbeatThreadLaunched())
        Sleep(500);
    Logger::StopPerformanceCounter(xorstr_("{D7310D1B-17C9-42D2-9511-29906528545E}"));

    Logger::StartPerformanceCounter(xorstr_("{6EE91FB8-257A-4AB5-83B6-FB93F8ADA554}"));
    auto sendAnticheatThread = MilkThread(reinterpret_cast<uintptr_t>(Communication::SendAnticheat));
    Logger::StopPerformanceCounter(xorstr_("{6EE91FB8-257A-4AB5-83B6-FB93F8ADA554}"));

    InitializeMaple();
    Logger::StopPerformanceCounter(xorstr_("{6907431E-A1F9-4E21-BD08-8CF5078CB8D1}"));

    VIRTUALIZER_TIGER_LITE_END

        return 0;
}

void InitializeMaple()
{
    VIRTUALIZER_FISH_RED_START
        Logger::StartPerformanceCounter(xorstr_("{66BF4512-01D6-4F5B-94C4-E48776FCE6B9}"));

    if (!Communication::GetIsConnected() || !Communication::GetIsHandshakeSucceeded() || !Communication::GetIsHeartbeatThreadLaunched())
        Security::CorruptMemory();

    Config::Initialize();

    Logger::Log(LogSeverity::Info, xorstr_("Initialization started."));

    bool goodKnownAuthVersion = AnticheatUtilities::IsRunningGoodKnownVersion();
    bool milkPrepared = Milk::Get().Prepare();

    if (!goodKnownAuthVersion || !milkPrepared)
        Config::Misc::ForceDisableScoreSubmission = true;

    if (goodKnownAuthVersion && !milkPrepared)
        Config::Misc::BypassFailed = true;

    VanillaResult vanillaResult = Vanilla::Initialize(true);
    if (vanillaResult == VanillaResult::Success)
    {
        Logger::Log(LogSeverity::Info, xorstr_("Initialized Vanilla!"));

        //initializing SDK
        Memory::StartInitialize();

        GameBase::Initialize();
        ErrorSubmission::Initialize();
        AudioEngine::Initialize();
        InputManager::Initialize();
        ModManager::Initialize();
        Obfuscated::Initialize();
        GameField::Initialize();
        Player::Initialize();
        Ruleset::Initialize();
        HitObjectManager::Initialize();
        BeatmapManager::Get().Initialize();
        Score::Initialize();
        StreamingManager::Initialize();
        DiscordRPC::Initialize();
        GLControl::Initialize();

        ReplayEditor::Editor::Initialize();

        Memory::EndInitialize();

        WaitForCriticalSDKToInitialize();

        //initializing UI and Spoofer
        //TODO: maybe we can move spoofer initialization outside of ui hooks?
        UI::Initialize();
    }
    else
    {
        Logger::Log(LogSeverity::Error, xorstr_("Vanilla failed to initialize with code %i"), (int)vanillaResult);

        Security::CorruptMemory();
    }
    Logger::StopPerformanceCounter(xorstr_("{66BF4512-01D6-4F5B-94C4-E48776FCE6B9}"));

    VIRTUALIZER_FISH_RED_END
}

void WaitForCriticalSDKToInitialize()
{
    VIRTUALIZER_FISH_RED_START
        Logger::StartPerformanceCounter(xorstr_("{EB5A207C-0E8B-4B27-9160-67B2271A2EE8}"));

    uintptr_t clientHash = Memory::Objects[xorstr_("GameBase::ClientHash")];
    uintptr_t updateTiming = Memory::Objects[xorstr_("GameBase::UpdateTiming")];

    VIRTUALIZER_FISH_RED_END

        unsigned int retries = 0;
#ifdef NO_BYPASS
    while (!clientHash || !updateTiming/*||!submit*/)
#else
    while (!clientHash || !updateTiming)
#endif
    {
        VIRTUALIZER_FISH_RED_START

            if (retries >= 30)
            {
                Logger::Log(LogSeverity::Error, xorstr_("Maple failed to initialize with code %i"), 0xdeadbeef);

                Security::CorruptMemory();
            }

        retries++;

        clientHash = Memory::Objects[xorstr_("GameBase::ClientHash")];
        updateTiming = Memory::Objects[xorstr_("GameBase::UpdateTiming")];

        Sleep(1000);

        VIRTUALIZER_FISH_RED_END
    }
    Logger::StopPerformanceCounter(xorstr_("{EB5A207C-0E8B-4B27-9160-67B2271A2EE8}"));
}