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
#include "SDK/Player/HitObjectManager.h"
#include "SDK/Player/Player.h"
#include "SDK/Player/Ruleset.h"
#include "SDK/Scoring/Score.h"
#include "SDK/Streaming/StreamingManager.h"
#include "UI/UI.h"
#include "Utilities/Anticheat/AnticheatUtilities.h"
#include "Utilities/Strings/StringUtilities.h"
#include "Dependencies/Milk/MilkThread.h"

#include <psapi.h>

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

LONG __stdcall pVectoredExceptionHandler(EXCEPTION_POINTERS* ExceptionInfo)
{
    if (ExceptionInfo->ExceptionRecord->ExceptionCode != EXCEPTION_ACCESS_VIOLATION)
        return EXCEPTION_CONTINUE_SEARCH;

    VIRTUALIZER_FISH_WHITE_START
    DWORD exceptionCode = ExceptionInfo->ExceptionRecord->ExceptionCode;
    PVOID exceptionAddress = ExceptionInfo->ExceptionRecord->ExceptionAddress;

    auto ctx = ExceptionInfo->ContextRecord;

    const char* exceptionMessageFormat = "Maple has encountered a crash!\n\nExceptionCode: 0x%08X ExceptionAddress: %p\nEAX: 0x%08X ECX: 0x%08X EDX: 0x%08X EBX: 0x%08X ESP: 0x%08X EBP: 0x%08X ESI: 0x%08X EDI: 0x%08X EIP: 0x%08X\n\nModules: \n%s\n\nFrame: \n%s\n\n";

    auto moduleMap = std::map<std::string, std::vector<MEMORY_BASIC_INFORMATION32>>();

    MEMORY_BASIC_INFORMATION32 mbi{};
    LPVOID address = nullptr;
    bool foundHeader = false;

    auto currentModule = std::vector<MEMORY_BASIC_INFORMATION32>();
    std::string moduleName = "";
    while (VirtualQuery(address, reinterpret_cast<PMEMORY_BASIC_INFORMATION>(&mbi), sizeof mbi) != 0)
    {
        address = reinterpret_cast<LPVOID>(mbi.BaseAddress + mbi.RegionSize);
        if (mbi.BaseAddress == NULL)
            continue;

        // Search for only PE Headers in memory
        if (mbi.AllocationProtect == PAGE_EXECUTE_WRITECOPY &&
            mbi.State == MEM_COMMIT &&
            mbi.Protect == PAGE_READONLY &&
            mbi.Type == MEM_IMAGE &&
            mbi.RegionSize == 0x1000)
        {
            PIMAGE_DOS_HEADER dosHeader = reinterpret_cast<PIMAGE_DOS_HEADER>(mbi.BaseAddress);
            if (dosHeader->e_magic != 0x5a4d)
                continue;

            char szDll[2048];
            DWORD result = GetMappedFileNameA(GetCurrentProcess(), address, szDll, sizeof(szDll));
            if (result == 0)
                continue;
            currentModule.emplace_back(mbi);
            moduleName = std::string(szDll, result);
            foundHeader = true;
        }

        if (foundHeader)
        {
            if (mbi.State == MEM_FREE && mbi.Protect == PAGE_NOACCESS) {
                foundHeader = false;
                moduleMap.insert(std::make_pair(moduleName, currentModule));

                currentModule.clear();
                moduleName = "";
                continue;
            }

            currentModule.emplace_back(mbi);
        }
    }

    const char* moduleFormat = "%s loaded at 0x%08X (size 0x%08X)\n";
    char moduleBuffer[2048];
    std::ostringstream modules;

    for (auto const& module : moduleMap)
    {
        DWORD base = module.second[0].BaseAddress;
        // Calculate size of module
        uint32_t size = 0;
        for (int i = 0; i < module.second.size(); i++)
        {
            auto region = module.second[i];
            size += region.RegionSize;
        }
        int stringLength = sprintf(moduleBuffer, moduleFormat, module.first.c_str(), base, size);
        modules << std::string(moduleBuffer, stringLength);
    }

    const int MAX_CALLERS = 62;
    PVOID callers[MAX_CALLERS];
    int count = CaptureStackBackTrace(0, MAX_CALLERS, callers, nullptr);

    const char* frameFormat = "0x%08X called from 0x%08X (%s)\n";
    char frameBuffer[2048];
    std::ostringstream stackFrame;
    for (int i = 0; i < count; i++) {
        DWORD caller = reinterpret_cast<DWORD>(callers[i]);
        std::string moduleName = "";
        // Find the caller in modules
        for (auto const& module : moduleMap)
        {
            DWORD base = module.second[0].BaseAddress;
            // Calculate size of module
            uint32_t size = 0;
            for (int i = 0; i < module.second.size(); i++)
            {
                auto region = module.second[i];
                size += region.RegionSize;
            }

            if (caller >= base && base + size >= caller)
            {
                moduleName = module.first;
                break;
            }
        }

        int stringLength = sprintf(frameBuffer, frameFormat, i, callers[i], moduleName.c_str());
        stackFrame << std::string(frameBuffer, stringLength);
    }

    char crashReportBuffer[1024 + modules.str().size() + stackFrame.str().size()];
    int stringLength = sprintf(crashReportBuffer, exceptionMessageFormat, exceptionCode, exceptionAddress,
        ctx->Eax, ctx->Ecx, ctx->Edx, ctx->Ebx, ctx->Esp, ctx->Ebp, ctx->Esi, ctx->Edi,
        ctx->Eip, modules.str().c_str(), stackFrame.str().c_str());
    [[clang::noinline]] Logger::WriteCrashReport(std::string(crashReportBuffer, stringLength));
    MessageBoxA(nullptr, xorstr_("Maple has encountered a crash and has created a crash report in the config directory!"), nullptr, MB_OK | MB_ICONERROR | MB_TOPMOST);
    ExitProcess(0xB00BB00B);
    VIRTUALIZER_FISH_WHITE_END
}

DWORD WINAPI Initialize()
{
    initializeThread->CleanCodeCave();
    delete initializeThread;

    auto data_addr = data;

    if (!data_addr)
        Security::CorruptMemory();

    VIRTUALIZER_FISH_WHITE_START
    [[clang::noinline]] AddVectoredExceptionHandler(false, pVectoredExceptionHandler);
    UserData userData = *static_cast<UserData*>(data_addr);
    Communication::SetUser(new User(userData.Username, userData.SessionToken, userData.DiscordID, userData.DiscordAvatarHash));

    memset(data_addr, 0x0, sizeof(UserData));

    Communication::Connect();

    int retries = 0;
    while (!Communication::GetIsConnected() || !Communication::GetIsHandshakeSucceeded() || !Communication::GetIsHeartbeatThreadLaunched())
    {
        if (retries >= 50)
        {
            Logger::Log(LogSeverity::Error, xorstr_("Maple has encountered a forged stack franme exception! Error code: %i"), 0xb00bb00b);

            Security::CorruptMemory();
        }
        retries++;
        Sleep(500);
    }

    auto sendAnticheatThread = MilkThread(reinterpret_cast<uintptr_t>(Communication::SendAnticheat));

    InitializeMaple();

    VIRTUALIZER_FISH_WHITE_END
    return 0;
}

void InitializeMaple()
{
    VIRTUALIZER_FISH_RED_START
    
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

    VIRTUALIZER_FISH_RED_END
    }

void WaitForCriticalSDKToInitialize()
{
    VIRTUALIZER_FISH_RED_START
    
    uintptr_t clientHash = Memory::Objects[xorstr_("GameBase::ClientHash")];
    uintptr_t updateTiming = Memory::Objects[xorstr_("GameBase::UpdateTiming")];
    uintptr_t submit = Memory::Objects[xorstr_("Score::Submit")];

    VIRTUALIZER_FISH_RED_END
    
    unsigned int retries = 0;
#ifdef NO_BYPASS
    while (!clientHash || !updateTiming/*||!submit*/)
#else
    while (!clientHash || !updateTiming || !submit)
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
        submit = Memory::Objects[xorstr_("Score::Submit")];

        Sleep(1000);

        VIRTUALIZER_FISH_RED_END
            }
}