#include <clocale>
#include <iostream>
#include <WinSock2.h>

#include "curl.h"
#include "ThemidaSDK.h"
#include "Vanilla.h"
#include "Communication/Communication.h"
#include "Hooking/VanillaHooking.h"

#include "Logging/Logger.h"
#include "Storage/Storage.h"
#include "Config/Config.h"

#include "Utilities/Security/xorstr.hpp"
#include "Utilities/Security/Security.h"

#include "SDK/Memory.h"
#include "SDK/Audio/AudioEngine.h"
#include "SDK/Discord/DiscordRPC.h"
#include "SDK/GL/GLControl.h"
#include "SDK/Helpers/ErrorSubmission.h"
#include "SDK/Helpers/Obfuscated.h"
#include "SDK/Helpers/pWebRequest.h"
#include "SDK/Input/InputManager.h"
#include "SDK/Mods/ModManager.h"
#include "SDK/Online/BanchoClient.h"
#include "SDK/Osu/GameBase.h"
#include "SDK/Osu/GameField.h"
#include "SDK/Player/HitObjectManager.h"
#include "SDK/Player/Player.h"
#include "SDK/Player/Ruleset.h"
#include "SDK/Scoring/Score.h"
#include "SDK/Streaming/StreamingManager.h"
#include "UI/UI.h"
#include "Utilities/Strings/StringUtilities.h"

DWORD WINAPI Initialize(LPVOID data_addr);
void InitializeMaple();

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
    VM_FISH_RED_START
    STR_ENCRYPT_START

    if (!Communication::EstablishedConnection || !Communication::HeartbeatThreadLaunched || !Communication::HandshakeSucceeded)
        Security::CorruptMemory();

    Storage::Initialize(Communication::CurrentUser->UsernameHashed);
    Config::Initialize();

	#ifdef _DEBUG
	    Logger::Initialize(LogSeverity::All, true, L"Runtime log | Maple");
	#else
	    Logger::Initialize(LogSeverity::Error | LogSeverity::Assert | LogSeverity::Warning);
	#endif

	Logger::Log(LogSeverity::Info, xor ("Initialization started."));

    VanillaResult vanillaResult = Vanilla::Initialize(true);
    if (vanillaResult == VanillaResult::Success)
    {
        Logger::Log(LogSeverity::Info, xor ("Initialized Vanilla!"));
		
        //initializing SDK
        Memory::StartInitialize();

        GameBase::Initialize();
        BanchoClient::Initialize();
        pWebRequest::Initialize();
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

        //initializing UI and Spoofer
        //TODO: maybe we can move spoofer initialization outside of ui hooks?
        UI::Initialize();
    }
    else
    {
        Logger::Log(LogSeverity::Error, xor ("Vanilla failed to initialize with code %i"), (int)vanillaResult);

        Security::CorruptMemory();
    }

    VM_FISH_RED_END
    STR_ENCRYPT_END
}
