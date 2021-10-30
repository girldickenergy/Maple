#include <iostream>
#include <thread>
#include <windows.h>
#include <clocale>

#include <ThemidaSDK.h>

#include "Communication/Communication.h"
#include "Config/Config.h"
#include "Features/Timewarp/Timewarp.h"
#include "Features/Visuals/VisualsSpoofers.h"
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

#define CRYPTOPP_ENABLE_NAMESPACE_WEAK 1
#include <md5.h>
#include <hex.h>

DWORD WINAPI Initialize(LPVOID data_addr);
void InitializeMaple(const std::string& username);
void InitializeLogging(const std::string& directory);
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
    VM_FISH_RED_START
    STR_ENCRYPT_START
	
    char* val;
    size_t len;
    errno_t err = _dupenv_s(&val, &len, xor ("APPDATA")); //TODO: xor later

    std::string path(val);

    CryptoPP::Weak1::MD5 hash;
    byte digest[CryptoPP::Weak1::MD5::DIGESTSIZE];

    hash.CalculateDigest(digest, (byte*)username.c_str(), username.length());

    CryptoPP::HexEncoder encoder;
    std::string usernameHashed;
    encoder.Attach(new CryptoPP::StringSink(usernameHashed));
    encoder.Put(digest, sizeof(digest));
    encoder.MessageEnd();
	
    path += "\\" + usernameHashed;

    VM_FISH_RED_END
    STR_ENCRYPT_END
    
    return path;
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
    //auto pArgs = (CustomArgs*)data_addr;

    //std::string data(pArgs->user_data, 255);

    //std::vector<std::string> split = StringUtilities::Split(data);

    Communication::CurrentUser->Username = "Maple Syrup";//split[0];
    Communication::CurrentUser->SessionID = "dicks";//split[1];
    //Communication::CurrentUser->DiscordID = split[2];
    //Communication::CurrentUser->AvatarHash = split[3];

    Communication::ConnectToServer();

    while (!Communication::EstablishedConnection || !Communication::HeartbeatThreadLaunched || !Communication::HandshakeSucceeded)
        Sleep(500);

    InitializeMaple(Communication::CurrentUser->Username);
    STR_ENCRYPT_END
    VM_SHARK_BLACK_END

    return 0;
}

void InitializeMaple(const std::string& username)
{
    if (!Communication::EstablishedConnection || !Communication::HeartbeatThreadLaunched || !Communication::HandshakeSucceeded)
        Security::CorruptMemory();
	
    std::string workingDirectory = GetWorkingDirectory(username);
	
    Vanilla::Initialize();

    InitializeLogging(workingDirectory);

    InitializeSdk();

    Config::Initialize(workingDirectory);

    Hooks::InstallAllHooks();

    StartFunctions();
}

void InitializeLogging(const std::string& directory)
{
    VM_FISH_RED_START
    STR_ENCRYPT_START
	
    if (!Communication::EstablishedConnection || !Communication::HeartbeatThreadLaunched || !Communication::HandshakeSucceeded)
        Security::CorruptMemory();
	
#ifdef _DEBUG
    Logger::Initialize(directory + "\\logs\\runtime.log", LogSeverity::Error | LogSeverity::Debug | LogSeverity::Assert | LogSeverity::Warning, true, NULL);
#else
    Logger::Initialize(directory + "\\logs\\runtime.log", LogSeverity::All);
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

    CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(Timewarp::TimewarpThread), nullptr, 0, nullptr);
    CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(VisualsSpoofers::FlashlightRemoverThread), nullptr, 0, nullptr);

    VM_FISH_RED_END
}
