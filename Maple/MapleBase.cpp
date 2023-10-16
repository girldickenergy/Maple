#include "MapleBase.h"

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

    TryHookSetMousePosition();
    TryHookMouseViaKeyboardControls();
    TryHookScoreSubmit();
    TryHookOnPlayerLoadComplete();
    TryHookPlayerDispose();

    TryHookSubmitError();

    TryFindPlayerFlag();
}

void MapleBase::OnJIT(uintptr_t address, unsigned int size)
{
    if (!oSetMousePosition)
        TryHookSetMousePosition(address, size);

    if (!oMouseViaKeyboardControls)
        TryHookMouseViaKeyboardControls(address, size);

    if (!oScoreSubmit)
        TryHookScoreSubmit(address, size);

    if (!oOnPlayerLoadComplete)
        TryHookOnPlayerLoadComplete(address, size);

    if (!oPlayerDispose)
        TryHookPlayerDispose(address, size);

    if (!oSubmitError)
        TryHookSubmitError(address, size);

    if (!m_PlayerFlag)
        TryFindPlayerFlag(address, size);

    for (const std::pair<std::string, std::shared_ptr<ISDK>> sdk : m_SDKs)
        sdk.second->OnJIT(address, size);

    for (const std::shared_ptr<IModule>& module : m_Modules)
        module->OnJIT(address, size);
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

void __fastcall MapleBase::ScoreSubmitHook(uintptr_t instance)
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

    if (allowSubmission)
        [[clang::musttail]] return oScoreSubmit(instance);
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

void __fastcall MapleBase::SubmitErrorHook(uintptr_t err)
{
    return;
}

void MapleBase::TryHookSetMousePosition(uintptr_t start, unsigned int size)
{
    if (const uintptr_t setMousePosition = start && size
         ? m_Vanilla->GetPatternScanner().FindPatternInRange(xorstr_("55 8B EC 83 EC 14 A1 ?? ?? ?? ?? 83 C0 04 D9 45 08 D9 18 D9 45 0C D9 58 04 A1 ?? ?? ?? ?? 83 C0 04 D9 00 D9 5D FC"), start, size)
         : m_Vanilla->GetPatternScanner().FindPattern(xorstr_("55 8B EC 83 EC 14 A1 ?? ?? ?? ?? 83 C0 04 D9 45 08 D9 18 D9 45 0C D9 58 04 A1 ?? ?? ?? ?? 83 C0 04 D9 00 D9 5D FC")))
    {
        if (m_Vanilla->GetHookManager().InstallHook(xorstr_("MouseManager.SetMousePosition"), setMousePosition, reinterpret_cast<uintptr_t>(SetMousePositionHook), reinterpret_cast<uintptr_t*>(&oSetMousePosition)) == VanillaResult::Success)
            m_RuntimeLogger->Log(LogLevel::Verbose, xorstr_("Hooked MouseManager.SetMousePosition"));
        else
            m_RuntimeLogger->Log(LogLevel::Error, xorstr_("Failed to hook MouseManager.SetMousePosition!"));
    }
}

void MapleBase::TryHookMouseViaKeyboardControls(uintptr_t start, unsigned int size)
{
    if (const uintptr_t mouseViaKeyboardControls = start && size
         ? m_Vanilla->GetPatternScanner().FindPatternInRange(xorstr_("55 8B EC 57 56 83 3D ?? ?? ?? ?? 02 74 04 5E 5F 5D C3 33 C9 FF 15 ?? ?? ?? ?? 8B F0 85 F6 0F 84"), start, size)
         : m_Vanilla->GetPatternScanner().FindPattern(xorstr_("55 8B EC 57 56 83 3D ?? ?? ?? ?? 02 74 04 5E 5F 5D C3 33 C9 FF 15 ?? ?? ?? ?? 8B F0 85 F6 0F 84")))
    {
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

        if (m_Vanilla->GetHookManager().InstallHook(xorstr_("InputManager.MouseViaKeyboardControls"), mouseViaKeyboardControls, reinterpret_cast<uintptr_t>(MouseViaKeyboardControlsHook), reinterpret_cast<uintptr_t*>(&oMouseViaKeyboardControls)) == VanillaResult::Success)
            m_RuntimeLogger->Log(LogLevel::Verbose, xorstr_("Hooked InputManager.MouseViaKeyboardControls"));
        else
            m_RuntimeLogger->Log(LogLevel::Error, xorstr_("Failed to hook InputManager.MouseViaKeyboardControls!"));
    }
}

void MapleBase::TryHookScoreSubmit(uintptr_t start, unsigned int size)
{
    if (const uintptr_t submit = start && size
         ? m_Vanilla->GetPatternScanner().FindPatternInRange(xorstr_("00 7E ?? 8B 3D ?? ?? ?? ?? C6 87 ?? ?? ?? ?? 00 8B 3D ?? ?? ?? ?? 8B CF 39 09 E8 ?? ?? ?? ?? 8B 8E"), start, size)
         : m_Vanilla->GetPatternScanner().FindPattern(xorstr_("00 7E ?? 8B 3D ?? ?? ?? ?? C6 87 ?? ?? ?? ?? 00 8B 3D ?? ?? ?? ?? 8B CF 39 09 E8 ?? ?? ?? ?? 8B 8E")))
    {
        oScoreSubmit = reinterpret_cast<fnScoreSubmit>(static_cast<intptr_t>(submit + 0x1B) + *reinterpret_cast<int*>(submit + 0x1B) + 0x4);
        *reinterpret_cast<intptr_t*>(submit + 0x1B) = reinterpret_cast<intptr_t>(ScoreSubmitHook) - static_cast<intptr_t>(submit + 0x1B) - 0x4;

        m_RuntimeLogger->Log(LogLevel::Verbose, xorstr_("Hooked Score.Submit"));
    }
}

void MapleBase::TryHookOnPlayerLoadComplete(uintptr_t start, unsigned int size)
{
    if (const uintptr_t onPlayerLoadComplete = start && size
         ? m_Vanilla->GetPatternScanner().FindPatternInRange(xorstr_("55 8B EC 57 56 53 83 EC 54 8B F1 8D 7D AC B9 ?? ?? ?? ?? 33 C0 F3 AB 8B CE 89 4D B0 33 C9 89 0D"), start, size)
         : m_Vanilla->GetPatternScanner().FindPattern(xorstr_("55 8B EC 57 56 53 83 EC 54 8B F1 8D 7D AC B9 ?? ?? ?? ?? 33 C0 F3 AB 8B CE 89 4D B0 33 C9 89 0D")))
    {
        if (m_Vanilla->GetHookManager().InstallHook(xorstr_("Player.OnLoadComplete"), onPlayerLoadComplete, reinterpret_cast<uintptr_t>(OnPlayerLoadCompleteHook), reinterpret_cast<uintptr_t*>(&oOnPlayerLoadComplete)) == VanillaResult::Success)
            m_RuntimeLogger->Log(LogLevel::Verbose, xorstr_("Hooked Player.OnLoadComplete"));
        else
            m_RuntimeLogger->Log(LogLevel::Error, xorstr_("Failed to hook Player.OnLoadComplete!"));
    }
}

void MapleBase::TryHookPlayerDispose(uintptr_t start, unsigned int size)
{
    if (const uintptr_t playerDispose = start && size
         ? m_Vanilla->GetPatternScanner().FindPatternInRange(xorstr_("55 8B EC 57 56 53 83 EC 08 89 55 F0 8B F1 8B 8E B8 00 00 00 85 C9 74 0D BA 01 00 00 00 8B 01 8B 40 28 FF 50 1C"), start, size)
         : m_Vanilla->GetPatternScanner().FindPattern(xorstr_("55 8B EC 57 56 53 83 EC 08 89 55 F0 8B F1 8B 8E B8 00 00 00 85 C9 74 0D BA 01 00 00 00 8B 01 8B 40 28 FF 50 1C")))
    {
        if (m_Vanilla->GetHookManager().InstallHook(xorstr_("Player.Dispose"), playerDispose, reinterpret_cast<uintptr_t>(PlayerDisposeHook), reinterpret_cast<uintptr_t*>(&oPlayerDispose)) == VanillaResult::Success)
            m_RuntimeLogger->Log(LogLevel::Verbose, xorstr_("Hooked Player.Dispose"));
        else
            m_RuntimeLogger->Log(LogLevel::Error, xorstr_("Failed to hook Player.Dispose!"));
    }
}

void MapleBase::TryHookSubmitError(uintptr_t start, unsigned int size)
{
    if (const uintptr_t submitError = start && size
         ? m_Vanilla->GetPatternScanner().FindPatternInRange(xorstr_("55 8B EC 57 56 83 EC 40 8B F1 8D 7D C0 B9 ?? ?? ?? ?? 33 C0 F3 AB 8B CE 89 4D C8 83 3D"), start, size)
         : m_Vanilla->GetPatternScanner().FindPattern(xorstr_("55 8B EC 57 56 83 EC 40 8B F1 8D 7D C0 B9 ?? ?? ?? ?? 33 C0 F3 AB 8B CE 89 4D C8 83 3D")))
    {
        if (m_Vanilla->GetHookManager().InstallHook(xorstr_("ErrorSubmission.Submit"), submitError, reinterpret_cast<uintptr_t>(SubmitErrorHook), reinterpret_cast<uintptr_t*>(&oSubmitError)) == VanillaResult::Success)
            m_RuntimeLogger->Log(LogLevel::Verbose, xorstr_("Hooked ErrorSubmission.Submit"));
        else
            m_RuntimeLogger->Log(LogLevel::Error, xorstr_("Failed to hook ErrorSubmission.Submit!"));
    }
}

void MapleBase::TryFindPlayerFlag(uintptr_t start, unsigned int size)
{
    if (const uintptr_t playerFlag = start && size
         ? m_Vanilla->GetPatternScanner().FindPatternInRange(xorstr_("E8 ?? ?? ?? ?? 33 D2 89 15 ?? ?? ?? ?? 88 15 ?? ?? ?? ?? B9"), start, size)
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
    if (!m_SDKs.contains(sdk->GetName()))
    {
        sdk->OnLoad(shared_from_this());
        m_SDKs[sdk->GetName()] = sdk;
    }
}

void MapleBase::AddSDKRange(const std::initializer_list<std::shared_ptr<ISDK>>& sdks)
{
    for (const std::shared_ptr<ISDK>& sdk : sdks)
        AddSDK(sdk);
}

std::shared_ptr<ISDK> MapleBase::GetSDK(const std::string& name)
{
    return m_SDKs.contains(name) ? m_SDKs[name] : nullptr;
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
