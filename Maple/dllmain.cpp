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

//move this somewhere else
std::string GetAuthPath()
{
    char buffer[MAX_PATH];
    GetModuleFileNameA(NULL, buffer, MAX_PATH);
    std::string::size_type pos = std::string(buffer).find_last_of(xor ("\\/"));

    return std::string(buffer).substr(0, pos) + xor ("\\osu!auth.dll");
}

std::string GetAuthHash()
{
    std::string hash;
    CryptoPP::SHA256 algo;
    CryptoPP::FileSource fs(GetAuthPath().c_str(), true, new CryptoPP::HashFilter(algo, new CryptoPP::HexEncoder(new CryptoPP::StringSink(hash))));

    return hash;
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

    if (std::filesystem::exists(GetAuthPath()))
    {
        const std::string authHash = GetAuthHash();
        if (authHash != xor ("FD8321C346DC33CD24D7AF22DB750ADC2F42D9C091B31A15587291DC086147FC") && authHash != xor ("176063779747AF3659FCFA4BC8BA01FFD9A6EA9BC4FCCA5A406A7D7CD9058318") && authHash != xor ("EA61F14A2FB494395887B83DACD80EF9BA7CCBF342EDD030387ADBE5807BA5A6") && authHash != xor ("C8862DA8AE15362FA7943BC96C35C04D4D2CF2C13D74EA1B475E6E391FAF1EF1") && authHash != xor ("8062D1A861DE8FC3693D3DAA7C69ECEA70CDB74A58C221E7ED7152E56ADE77EB") && authHash != xor ("64DCBC6BC55853D031C6621DA3B538CFE93D079D330CADBB6F6F90D3727E8D09"))
            Config::Misc::ForceDisableScoreSubmission = true;
	}

	#ifdef _DEBUG
	    Logger::Initialize(LogSeverity::All, false, true, L"Runtime log | Maple");
	#else
	    Logger::Initialize(LogSeverity::All, true);
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

        uintptr_t failSafe = Memory::Objects[xor ("GameBase::ClientHash")];
        unsigned int retries = 0;
        while (!failSafe)
        {
            if (retries >= 15)
            {
                Logger::Log(LogSeverity::Error, xor ("Maple failed to initialize with code %i"), 0xdeadbeef);

                Security::CorruptMemory();
            }

            retries++;
            failSafe = Memory::Objects[xor ("GameBase::ClientHash")];

            Sleep(1000);
        }

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
