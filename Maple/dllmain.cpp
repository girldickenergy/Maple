#include <clocale>
#include <WinSock2.h>

#include "curl.h"
#include "ThemidaSDK.h"
#include "Vanilla.h"
#include "Communication/Communication.h"
#include "Hooking/VanillaHooking.h"

#include "Logging/Logger.h"
#include "Storage/Storage.h"
#include "Config/Config.h"
#include "Dependencies/Milk/Milk.h"

#include "Utilities/Security/xorstr.hpp"
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
#include "SDK/Player/HitObjectManager.h"
#include "SDK/Player/Player.h"
#include "SDK/Player/Ruleset.h"
#include "SDK/Scoring/Score.h"
#include "SDK/Streaming/StreamingManager.h"
#include "UI/UI.h"
#include "Utilities/Anticheat/AnticheatUtilities.h"
#include "Utilities/Strings/StringUtilities.h"
#include "Dependencies/Milk/MilkThread.h"

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
    VM_SHARK_BLACK_START
    STR_ENCRYPT_START

    initializeThread->CleanCodeCave();
    delete initializeThread;

	auto data_addr = data;

    int protectionVar = 0x501938CA;
    CHECK_PROTECTION(protectionVar, 0x9CCC379)
    if (protectionVar != 0x9CCC379)
        Security::CorruptMemory();

    if (!data_addr)
        Security::CorruptMemory();

    UserData userData = *static_cast<UserData*>(data_addr);
    Communication::SetUser(new User(userData.Username, userData.SessionToken, userData.DiscordID, userData.DiscordAvatarHash));

    memset(data_addr, 0x0, sizeof(UserData));

    Communication::Connect();

    while (!Communication::GetIsConnected() || !Communication::GetIsHandshakeSucceeded() || !Communication::GetIsHeartbeatThreadLaunched())
        Sleep(500);

    auto sendAnticheatThread = MilkThread(reinterpret_cast<uintptr_t>(Communication::SendAnticheat));

    InitializeMaple();

    STR_ENCRYPT_END
    VM_SHARK_BLACK_END

    return 0;
}

void InitializeMaple()
{
    VM_FISH_RED_START
    STR_ENCRYPT_START

    if (!Communication::GetIsConnected() || !Communication::GetIsHandshakeSucceeded() || !Communication::GetIsHeartbeatThreadLaunched())
        Security::CorruptMemory();

    Storage::Initialize(Communication::GetUser()->GetUsernameHashed());
    Config::Initialize();

#ifdef _DEBUG
    Logger::Initialize(LogSeverity::All, false, true, L"Runtime log | Maple");
#else
    Logger::Initialize(LogSeverity::All, true);
#endif

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
        Score::Initialize();
        StreamingManager::Initialize();
        DiscordRPC::Initialize();
        GLControl::Initialize();

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

    VM_FISH_RED_END
    STR_ENCRYPT_END
}

void WaitForCriticalSDKToInitialize()
{
    VM_FISH_RED_START
    STR_ENCRYPT_START

    uintptr_t clientHash = Memory::Objects[xorstr_("GameBase::ClientHash")];
    uintptr_t updateTiming = Memory::Objects[xorstr_("GameBase::UpdateTiming")];
    uintptr_t submit = Memory::Objects[xorstr_("Score::Submit")];

    VM_FISH_RED_END
    STR_ENCRYPT_END

    unsigned int retries = 0;
    while (!clientHash || !updateTiming || !submit)
    {
        VM_FISH_RED_START
        STR_ENCRYPT_START

        if (retries >= 30)
        {
            Logger::Log(LogSeverity::Error, xorstr_("Maple failed to initialize with code %i"), 0xdeadbeef);

            Security::CorruptMemory();
        }

        retries++;

        clientHash = Memory::Objects[xorstr_("GameBase::ClientHash")];
        updateTiming = Memory::Objects[xorstr_("GameBase::UpdateTiming")];
        submit = Memory::Objects[xorstr_("Score::Submit")];

        Sleep(1000);

        VM_FISH_RED_END
        STR_ENCRYPT_END
    }
}
