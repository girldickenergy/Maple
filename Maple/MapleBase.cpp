#include "MapleBase.h"

#include "fnv1a.h"
#include "xorstr.h"

#include "Modules/IModule.h"
#include "SDK/ISDK.h"

void MapleBase::InitializeStorage(const std::string& directoryName)
{
    char* val;
    size_t len;
    errno_t err = _dupenv_s(&val, &len, xorstr_("APPDATA"));
    std::string storageDirectory = std::string(val) + xorstr_("\\") + directoryName;

    m_Storage = std::make_shared<Storage>(storageDirectory);
}

void MapleBase::InitializeLogging()
{
    AllocConsole();
    freopen_s(reinterpret_cast<FILE**>(stdout), xorstr_("CONOUT$"), xorstr_("w"), stdout);
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);

    LoggerInfo loggerInfo = LoggerInfo(xorstr_("mlo-12102023"), xorstr_("somehash"), xorstr_("someauthhash"), xorstr_("Windows 11"), xorstr_("v4.0.30319"));
    m_RuntimeLogger = std::make_shared<Logger>(m_Storage, xorstr_("runtime"), loggerInfo, LogLevel::Debug, false, consoleHandle);
}

void MapleBase::InitializeCore()
{
    m_Vanilla = std::make_shared<Vanilla>();
    VanillaResult vanillaResult = m_Vanilla->Initialize(true);
    if (vanillaResult != VanillaResult::Success)
    {
        m_RuntimeLogger->Log(LogLevel::Error, xorstr_("Vanilla failed to initialize with code %i"), static_cast<int>(vanillaResult));

        return; // todo: crash
    }

    m_Vanilla->SetJITCallback(OnJIT);

    TryHookOnPlayerLoadComplete();
    TryHookPlayerDispose();
    TryHookHandleScoreSubmission();
    TryHookSetMousePosition();
    TryHookMouseViaKeyboardControls();

    TryFindPlayerFlag();
}

void MapleBase::OnJIT(void* methodDesc, uintptr_t functionAddress, size_t functionSize)
{
    if (!oOnPlayerLoadComplete)
        TryHookOnPlayerLoadComplete(methodDesc, functionAddress, functionSize);

    if (!oPlayerDispose)
        TryHookPlayerDispose(methodDesc, functionAddress, functionSize);

    if (!oHandleScoreSubmission)
        TryHookHandleScoreSubmission(methodDesc, functionAddress, functionSize);

    if (!oSetMousePosition)
        TryHookSetMousePosition(methodDesc, functionAddress, functionSize);

    if (!oMouseViaKeyboardControls)
        TryHookMouseViaKeyboardControls(methodDesc, functionAddress, functionSize); // todo: replace with some lower level keyboard hook to add support for non-std modes

    if (!m_PlayerFlag)
        TryFindPlayerFlag(methodDesc, functionAddress, functionSize);

    // todo: implement new OnJIT for sdks and modules
    for (const std::pair<uint32_t, std::shared_ptr<ISDK>> sdk : m_SDKs)
        sdk.second->OnJIT(methodDesc, functionAddress, functionSize);

    for (const std::shared_ptr<IModule>& module : m_Modules)
        module->OnJIT(methodDesc, functionAddress, functionSize);
}

int __fastcall MapleBase::OnPlayerLoadCompleteHook(uintptr_t instance, bool success)
{
    if (success)
    {
        for (const std::shared_ptr<IModule>& module : m_Modules)
            module->OnPlayerLoad();

        m_IsPlayerLoaded = true;
    }

    [[clang::musttail]] return oOnPlayerLoadComplete(instance, success);
}

void __fastcall MapleBase::PlayerDisposeHook(uintptr_t instance, int disposing)
{
    for (const std::shared_ptr<IModule>& module : m_Modules)
        module->OnPlayerExit();

    m_IsPlayerLoaded = false;

    [[clang::musttail]] return oPlayerDispose(instance, disposing);
}

void __fastcall MapleBase::HandleScoreSubmissionHook(uintptr_t instance)
{
    if (m_PlayerFlag)
        *m_PlayerFlag = 0;
    else
    {
        m_ScoreSubmissionUnsafe = true;

        m_RuntimeLogger->Log(LogLevel::Important, xorstr_("Failed to reset Player.flag because it was null! Invalidating score submission."));
    }

    bool allowSubmission = !m_ScoreSubmissionUnsafe;

    for (const std::shared_ptr<IModule>& module : m_Modules)
        if (module->RequiresScoreSubmission())
            allowSubmission &= module->OnScoreSubmission();

    if (allowSubmission) [[clang::musttail]]
        return oHandleScoreSubmission(instance);
}

void __fastcall MapleBase::SetMousePositionHook(Vector2 position)
{
    Vector2 newPosition = position;

    if (m_IsPlayerLoaded)
    {
        for (const std::shared_ptr<IModule>& module : m_Modules)
            if (module->RequiresCursorPosition())
                newPosition = module->OnCursorPositionUpdate(newPosition);
    }

    [[clang::musttail]] return oSetMousePosition(newPosition);
}

[[clang::optnone]] void __fastcall MapleBase::MouseViaKeyboardControlsHook()
{
    if (m_IsPlayerLoaded)
    {
	const bool m1PressedPrevious = m_LeftButtons[0];
        const bool k1PressedPrevious = m_LeftButtons[2];
        const bool m2PressedPrevious = m_RightButtons[0];
        const bool k2PressedPrevious = m_RightButtons[2];

	oMouseViaKeyboardControls();

        auto newKeyState = OsuKeys::None;

	if (m_LeftButtons)
        {
            if (!m_LeftButtons[2] && m_LeftButtons[0])
                newKeyState |= OsuKeys::M1;
            else if (m_LeftButtons[2])
                newKeyState |= OsuKeys::K1;
        }

        if (m_RightButtons)
        {
            if (!m_RightButtons[2] && m_RightButtons[0])
                newKeyState |= OsuKeys::M2;
            else if (m_RightButtons[2])
                newKeyState |= OsuKeys::K2;
        }

        for (const std::shared_ptr<IModule>& module : m_Modules)
            if (module->RequiresGameplayKeys())
                newKeyState = module->OnGameplayKeysUpdate(newKeyState);

        const bool m1Pressed = (newKeyState & OsuKeys::M1) > OsuKeys::None;
        const bool k1Pressed = (newKeyState & OsuKeys::K1) > OsuKeys::None;
        const bool m2Pressed = (newKeyState & OsuKeys::M2) > OsuKeys::None;
        const bool k2Pressed = (newKeyState & OsuKeys::K2) > OsuKeys::None;

        if (m_LeftButtons)
        {
            m_LeftButtons[0] = m1Pressed || k1Pressed;
            m_LeftButtons[1] = (m1Pressed || k1Pressed) && ((m_MouseButtonInstantRelease ? !*m_MouseButtonInstantRelease : true) || !m1PressedPrevious);

	    m_LeftButtons[2] = k1Pressed;
            m_LeftButtons[3] = k1Pressed && ((m_MouseButtonInstantRelease ? !*m_MouseButtonInstantRelease : true) || !k1PressedPrevious);
        }

        if (m_RightButtons)
        {
            m_RightButtons[0] = m2Pressed || k2Pressed;
            m_RightButtons[1] = (m2Pressed || k2Pressed) && ((m_MouseButtonInstantRelease ? !*m_MouseButtonInstantRelease : true) || !m2PressedPrevious);

            m_RightButtons[2] = k2Pressed;
            m_RightButtons[3] = k2Pressed && ((m_MouseButtonInstantRelease ? !*m_MouseButtonInstantRelease : true) || !k2PressedPrevious);
        }

        if (m_LeftButton)
            *m_LeftButton = (m_LeftButtons[3] || m_LeftButtons[1]);

        if (m_RightButton)
            *m_RightButton = (m_RightButtons[3] || m_RightButtons[1]);
    }
    else
	[[clang::musttail]] return oMouseViaKeyboardControls();
}

void MapleBase::TryHookOnPlayerLoadComplete(void* methodDesc, uintptr_t functionAddress, size_t functionSize)
{
    const uintptr_t onPlayerLoadComplete = methodDesc
        ? m_Vanilla->GetPatternScanner().FindPatternInRange(xorstr_("55 8B EC 57 56 53 83 EC 54 8B F1 8D 7D AC B9 ?? ?? ?? ?? 33 C0 F3 AB 8B CE 89 4D B0 33 C9 89 0D"), functionAddress, functionSize)
        : m_Vanilla->GetPatternScanner().FindPattern(xorstr_("55 8B EC 57 56 53 83 EC 54 8B F1 8D 7D AC B9 ?? ?? ?? ?? 33 C0 F3 AB 8B CE 89 4D B0 33 C9 89 0D"));

    if (!onPlayerLoadComplete)
        return;

    if (!methodDesc)
        methodDesc = m_Vanilla->GetMethodDesc(onPlayerLoadComplete);

    if (!methodDesc)
    {
        m_RuntimeLogger->Log(LogLevel::Error, xorstr_("Failed to retrieve method descriptor of Player.OnLoadComplete!"));

        return;
    }

    if (m_Vanilla->GetHookManager().InstallPointerRedirectionHook(xorstr_("Player.OnLoadComplete"), m_Vanilla->GetMethodSlotAddress(methodDesc), reinterpret_cast<uintptr_t>(OnPlayerLoadCompleteHook), reinterpret_cast<uintptr_t*>(&oOnPlayerLoadComplete)) == VanillaResult::Success)
        m_RuntimeLogger->Log(LogLevel::Verbose, xorstr_("Hooked Player.OnLoadComplete"));
    else
        m_RuntimeLogger->Log(LogLevel::Error, xorstr_("Failed to hook Player.OnLoadComplete!"));
}

void MapleBase::TryHookPlayerDispose(void* methodDesc, uintptr_t functionAddress, size_t functionSize)
{
    const uintptr_t playerDispose = methodDesc
        ? m_Vanilla->GetPatternScanner().FindPatternInRange(xorstr_("55 8B EC 57 56 53 83 EC 08 89 55 F0 8B F1 8B 8E B8 00 00 00 85 C9 74 0D BA 01 00 00 00 8B 01 8B 40 28 FF 50 1C"), functionAddress, functionSize)
        : m_Vanilla->GetPatternScanner().FindPattern(xorstr_("55 8B EC 57 56 53 83 EC 08 89 55 F0 8B F1 8B 8E B8 00 00 00 85 C9 74 0D BA 01 00 00 00 8B 01 8B 40 28 FF 50 1C"));

    if (!playerDispose)
        return;

    if (!methodDesc)
        methodDesc = m_Vanilla->GetMethodDesc(playerDispose);

    if (!methodDesc)
    {
        m_RuntimeLogger->Log(LogLevel::Error, xorstr_("Failed to retrieve method descriptor of Player.Dispose!"));

        return;
    }

    if (m_Vanilla->GetHookManager().InstallPointerRedirectionHook(xorstr_("Player.Dispose"), m_Vanilla->GetMethodSlotAddress(methodDesc), reinterpret_cast<uintptr_t>(PlayerDisposeHook), reinterpret_cast<uintptr_t*>(&oPlayerDispose)) == VanillaResult::Success)
        m_RuntimeLogger->Log(LogLevel::Verbose, xorstr_("Hooked Player.Dispose"));
    else
        m_RuntimeLogger->Log(LogLevel::Error, xorstr_("Failed to hook Player.Dispose!"));
}

void MapleBase::TryHookHandleScoreSubmission(void* methodDesc, uintptr_t functionAddress, size_t functionSize)
{
    const uintptr_t handleScoreSubmission = methodDesc
        ? m_Vanilla->GetPatternScanner().FindPatternInRange(xorstr_("55 8B EC 57 56 53 83 EC 08 8B F1 80 BE ?? ?? ?? ?? 00 75 26 B9"), functionAddress, functionSize)
        : m_Vanilla->GetPatternScanner().FindPattern(xorstr_("55 8B EC 57 56 53 83 EC 08 8B F1 80 BE ?? ?? ?? ?? 00 75 26 B9"));

    if (!handleScoreSubmission)
        return;

    if (!methodDesc)
        methodDesc = m_Vanilla->GetMethodDesc(handleScoreSubmission);

    if (!methodDesc)
    {
        m_RuntimeLogger->Log(LogLevel::Error, xorstr_("Failed to retrieve method descriptor of Player.HandleScoreSubmission!"));

        return;
    }

    if (m_Vanilla->GetHookManager().InstallPointerRedirectionHook(xorstr_("Player.HandleScoreSubmission"), m_Vanilla->GetMethodSlotAddress(methodDesc), reinterpret_cast<uintptr_t>(HandleScoreSubmissionHook), reinterpret_cast<uintptr_t*>(&oHandleScoreSubmission)) == VanillaResult::Success)
        m_RuntimeLogger->Log(LogLevel::Verbose, xorstr_("Hooked Player.HandleScoreSubmission"));
    else
        m_RuntimeLogger->Log(LogLevel::Error, xorstr_("Failed to hook Player.HandleScoreSubmission!"));
}

void MapleBase::TryHookSetMousePosition(void* methodDesc, uintptr_t functionAddress, size_t functionSize)
{
    const uintptr_t setMousePosition = methodDesc
        ? m_Vanilla->GetPatternScanner().FindPatternInRange(xorstr_("55 8B EC 83 EC 14 A1 ?? ?? ?? ?? 83 C0 04 D9 45 08 D9 18 D9 45 0C D9 58 04 A1 ?? ?? ?? ?? 83 C0 04 D9 00 D9 5D FC"), functionAddress, functionSize)
        : m_Vanilla->GetPatternScanner().FindPattern(xorstr_("55 8B EC 83 EC 14 A1 ?? ?? ?? ?? 83 C0 04 D9 45 08 D9 18 D9 45 0C D9 58 04 A1 ?? ?? ?? ?? 83 C0 04 D9 00 D9 5D FC"));

    if (!setMousePosition)
        return;

    if (!methodDesc)
        methodDesc = m_Vanilla->GetMethodDesc(setMousePosition);

    if (!methodDesc)
    {
        m_RuntimeLogger->Log(LogLevel::Error, xorstr_("Failed to retrieve method descriptor of Player.OnLoadComplete!"));

        return;
    }

    if (m_Vanilla->GetHookManager().InstallPointerRedirectionHook(xorstr_("MouseManager.SetMousePosition"), m_Vanilla->GetMethodSlotAddress(methodDesc), reinterpret_cast<uintptr_t>(SetMousePositionHook), reinterpret_cast<uintptr_t*>(&oSetMousePosition)) == VanillaResult::Success)
        m_RuntimeLogger->Log(LogLevel::Verbose, xorstr_("Hooked MouseManager.SetMousePosition"));
    else
        m_RuntimeLogger->Log(LogLevel::Error, xorstr_("Failed to hook MouseManager.SetMousePosition!"));
}

void MapleBase::TryHookMouseViaKeyboardControls(void* methodDesc, uintptr_t functionAddress, size_t functionSize)
{
    const uintptr_t mouseViaKeyboardControls = methodDesc
        ? m_Vanilla->GetPatternScanner().FindPatternInRange(xorstr_("55 8B EC 57 56 83 3D ?? ?? ?? ?? 02 74 04 5E 5F 5D C3 33 C9 FF 15 ?? ?? ?? ?? 8B F0 85 F6 0F 84"), functionAddress, functionSize)
        : m_Vanilla->GetPatternScanner().FindPattern(xorstr_("55 8B EC 57 56 83 3D ?? ?? ?? ?? 02 74 04 5E 5F 5D C3 33 C9 FF 15 ?? ?? ?? ?? 8B F0 85 F6 0F 84"));

    if (!mouseViaKeyboardControls)
        return;

    if (!methodDesc)
        methodDesc = m_Vanilla->GetMethodDesc(mouseViaKeyboardControls);

    if (!methodDesc)
    {
        m_RuntimeLogger->Log(LogLevel::Error, xorstr_("Failed to retrieve method descriptor of Player.OnLoadComplete!"));

        return;
    }

    if (const uintptr_t mouseButtonInstantRelease = m_Vanilla->GetPatternScanner().FindPatternInRange(xorstr_("85 C9 74 45 80 3D ?? ?? ?? ?? 00"), mouseViaKeyboardControls + 0x30, mouseViaKeyboardControls + 0xB0))
        m_MouseButtonInstantRelease = *reinterpret_cast<bool**>(mouseButtonInstantRelease + 0x6);

    if (const uintptr_t leftButtons = m_Vanilla->GetPatternScanner().FindPatternInRange(xorstr_("85 F6 0F 84 ?? ?? ?? ?? 0F B6 3D"), mouseViaKeyboardControls, mouseViaKeyboardControls + 0x50))
        m_LeftButtons = *reinterpret_cast<bool**>(leftButtons + 0xB);

    if (const uintptr_t rightButtons = m_Vanilla->GetPatternScanner().FindPatternInRange(xorstr_("85 F6 0F 84 ?? ?? ?? ?? 0F B6 3D"), mouseViaKeyboardControls + 0x150, mouseViaKeyboardControls + 0x50))
        m_RightButtons = *reinterpret_cast<bool**>(rightButtons + 0xB);

    if (const uintptr_t leftButton = m_Vanilla->GetPatternScanner().FindPatternInRange(xorstr_("74 09 83 3D ?? ?? ?? ?? 00 75 0A C7 05"), mouseViaKeyboardControls + 0x2A0, mouseViaKeyboardControls + 0x60))
        m_LeftButton = *reinterpret_cast<int**>(leftButton + 0xD);

    if (const uintptr_t rightButton = m_Vanilla->GetPatternScanner().FindPatternInRange(xorstr_("74 09 83 3D ?? ?? ?? ?? 00 75 0A C7 05"), mouseViaKeyboardControls + 0x310, mouseViaKeyboardControls + 0x2A))
        m_RightButton = *reinterpret_cast<int**>(rightButton + 0xD);

    if (m_Vanilla->GetHookManager().InstallPointerRedirectionHook(xorstr_("InputManager.MouseViaKeyboardControls"), m_Vanilla->GetMethodSlotAddress(methodDesc), reinterpret_cast<uintptr_t>(MouseViaKeyboardControlsHook), reinterpret_cast<uintptr_t*>(&oMouseViaKeyboardControls)) == VanillaResult::Success)
        m_RuntimeLogger->Log(LogLevel::Verbose, xorstr_("InputManager.MouseViaKeyboardControls"));
    else
        m_RuntimeLogger->Log(LogLevel::Error, xorstr_("Failed to hook InputManager.MouseViaKeyboardControls!"));
}

void MapleBase::TryFindPlayerFlag(void* methodDesc, uintptr_t functionAddress, size_t functionSize)
{
    if (const uintptr_t playerFlag = methodDesc
         ? m_Vanilla->GetPatternScanner().FindPatternInRange(xorstr_("E8 ?? ?? ?? ?? 33 D2 89 15 ?? ?? ?? ?? 88 15 ?? ?? ?? ?? B9"), functionAddress, functionSize)
         : m_Vanilla->GetPatternScanner().FindPattern(xorstr_("E8 ?? ?? ?? ?? 33 D2 89 15 ?? ?? ?? ?? 88 15 ?? ?? ?? ?? B9")))
    {
        m_PlayerFlag = *reinterpret_cast<int**>(playerFlag + 0x9);

        m_RuntimeLogger->Log(LogLevel::Verbose, xorstr_("Found Player.flag"));
    }
}

void MapleBase::Initialize()
{
    InitializeStorage(xorstr_("Maple"));
    InitializeLogging();
    InitializeCore();
}

void MapleBase::AddSDK(const std::shared_ptr<ISDK>& sdk)
{
    if (!m_SDKs.contains(fnv1a::Hash(sdk->GetName())))
    {
        sdk->OnLoad(shared_from_this());
        m_SDKs[fnv1a::Hash(sdk->GetName())] = sdk;
    }
}

void MapleBase::AddSDKRange(const std::initializer_list<std::shared_ptr<ISDK>>& sdks)
{
    for (const std::shared_ptr<ISDK>& sdk : sdks)
        AddSDK(sdk);
}

std::shared_ptr<ISDK> MapleBase::GetSDK(const char* name)
{
    return m_SDKs.contains(fnv1a::Hash(name)) ? m_SDKs[fnv1a::Hash(name)] : nullptr;
}

void MapleBase::AddModule(const std::shared_ptr<IModule>& module)
{
    module->OnLoad(shared_from_this());
    m_Modules.push_back(module);
}

void MapleBase::AddModuleRange(const std::initializer_list<std::shared_ptr<IModule>>& modules)
{
    for (const std::shared_ptr<IModule>& module : modules)
        AddModule(module);
}

void MapleBase::RenderModulesGUI()
{
    for (const std::shared_ptr<IModule>& module : m_Modules)
        module->OnRender();
}

void MapleBase::MakeScoreSubmissionUnsafe()
{
    m_ScoreSubmissionUnsafe = true;
}

bool MapleBase::GetIsScoreSubmissionUnsafe()
{
    return m_ScoreSubmissionUnsafe;
}

std::shared_ptr<Logger> MapleBase::GetRuntimeLogger()
{
    return m_RuntimeLogger;
}

std::shared_ptr<Vanilla> MapleBase::GetVanilla()
{
    return m_Vanilla;
}
