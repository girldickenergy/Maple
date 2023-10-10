#include "MapleBase.h"

#include "xorstr.h"

#include "Modules/IModule.h"
#include "SDK/ISDK.h"

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

void __fastcall MapleBase::MouseViaKeyboardControlsHook()
{
    OsuKeys newKeys = OsuKeys::None;

    if (m_IsPlayerLoaded)
    {
        for (const std::shared_ptr<IModule>& module : m_Modules)
            if (module->RequiresGameplayKeys())
                newKeys = module->OnGameplayKeysUpdate(newKeys);
    }

    // todo: handle keys

    [[clang::musttail]] return oMouseViaKeyboardControls();
}

void __fastcall MapleBase::ScoreSubmitHook(uintptr_t instance)
{
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

void MapleBase::TryHookSetMousePosition(uintptr_t start, unsigned int size)
{
    // todo: error handling

    if (const uintptr_t setMousePosition = start && size
         ? m_Vanilla->GetPatternScanner().FindPatternInRange(xorstr_("55 8B EC 83 EC 14 A1 ?? ?? ?? ?? 83 C0 04 D9 45 08 D9 18 D9 45 0C D9 58 04 A1 ?? ?? ?? ?? 83 C0 04 D9 00 D9 5D FC"), start, size)
         : m_Vanilla->GetPatternScanner().FindPattern(xorstr_("55 8B EC 83 EC 14 A1 ?? ?? ?? ?? 83 C0 04 D9 45 08 D9 18 D9 45 0C D9 58 04 A1 ?? ?? ?? ?? 83 C0 04 D9 00 D9 5D FC")))
    {
        m_Vanilla->GetHookManager().InstallHook(xorstr_("MouseManager.SetMousePosition"), setMousePosition, reinterpret_cast<uintptr_t>(SetMousePositionHook), reinterpret_cast<uintptr_t*>(&oSetMousePosition));
    }
}

void MapleBase::TryHookMouseViaKeyboardControls(uintptr_t start, unsigned int size)
{
    // todo: error handling

    if (const uintptr_t mouseViaKeyboardControls = start && size
         ? m_Vanilla->GetPatternScanner().FindPatternInRange(xorstr_("55 8B EC 57 56 83 3D ?? ?? ?? ?? 02 74 04 5E 5F 5D C3 33 C9 FF 15 ?? ?? ?? ?? 8B F0 85 F6 0F 84"), start, size)
         : m_Vanilla->GetPatternScanner().FindPattern(xorstr_("55 8B EC 57 56 83 3D ?? ?? ?? ?? 02 74 04 5E 5F 5D C3 33 C9 FF 15 ?? ?? ?? ?? 8B F0 85 F6 0F 84")))
    {
        m_Vanilla->GetHookManager().InstallHook(xorstr_("InputManager.MouseViaKeyboardControls"), mouseViaKeyboardControls, reinterpret_cast<uintptr_t>(MouseViaKeyboardControlsHook), reinterpret_cast<uintptr_t*>(&oMouseViaKeyboardControls));
    }
}

void MapleBase::TryHookScoreSubmit(uintptr_t start, unsigned int size)
{
    // todo: error handling

    if (const uintptr_t submit = start && size
         ? m_Vanilla->GetPatternScanner().FindPatternInRange(xorstr_("00 7E ?? 8B 3D ?? ?? ?? ?? C6 87 ?? ?? ?? ?? 00 8B 3D ?? ?? ?? ?? 8B CF 39 09 E8 ?? ?? ?? ?? 8B 8E"), start, size)
         : m_Vanilla->GetPatternScanner().FindPattern(xorstr_("00 7E ?? 8B 3D ?? ?? ?? ?? C6 87 ?? ?? ?? ?? 00 8B 3D ?? ?? ?? ?? 8B CF 39 09 E8 ?? ?? ?? ?? 8B 8E")))
    {
        oScoreSubmit = reinterpret_cast<fnScoreSubmit>(static_cast<intptr_t>(submit + 0x1B) + *reinterpret_cast<int*>(submit + 0x1B) + 0x4);
        *reinterpret_cast<intptr_t*>(submit + 0x1B) = reinterpret_cast<intptr_t>(ScoreSubmitHook) - static_cast<intptr_t>(submit + 0x1B) - 0x4;
    }
}

void MapleBase::TryHookOnPlayerLoadComplete(uintptr_t start, unsigned int size)
{
    // todo: error handling

    if (const uintptr_t onPlayerLoadComplete = start && size
         ? m_Vanilla->GetPatternScanner().FindPatternInRange(xorstr_("55 8B EC 57 56 53 83 EC 54 8B F1 8D 7D AC B9 ?? ?? ?? ?? 33 C0 F3 AB 8B CE 89 4D B0 33 C9 89 0D"), start, size)
         : m_Vanilla->GetPatternScanner().FindPattern(xorstr_("55 8B EC 57 56 53 83 EC 54 8B F1 8D 7D AC B9 ?? ?? ?? ?? 33 C0 F3 AB 8B CE 89 4D B0 33 C9 89 0D")))
    {
        m_Vanilla->GetHookManager().InstallHook(xorstr_("Player.OnLoadComplete"), onPlayerLoadComplete, reinterpret_cast<uintptr_t>(OnPlayerLoadCompleteHook), reinterpret_cast<uintptr_t*>(&oOnPlayerLoadComplete));
    }
}

void MapleBase::TryHookPlayerDispose(uintptr_t start, unsigned int size)
{
    // todo: error handling

    if (const uintptr_t playerDispose = start && size
         ? m_Vanilla->GetPatternScanner().FindPatternInRange(xorstr_("55 8B EC 57 56 53 83 EC 08 89 55 F0 8B F1 8B 8E B8 00 00 00 85 C9 74 0D BA 01 00 00 00 8B 01 8B 40 28 FF 50 1C"), start, size)
         : m_Vanilla->GetPatternScanner().FindPattern(xorstr_("55 8B EC 57 56 53 83 EC 08 89 55 F0 8B F1 8B 8E B8 00 00 00 85 C9 74 0D BA 01 00 00 00 8B 01 8B 40 28 FF 50 1C")))
    {
        m_Vanilla->GetHookManager().InstallHook(xorstr_("Player.Dispose"), playerDispose, reinterpret_cast<uintptr_t>(PlayerDisposeHook), reinterpret_cast<uintptr_t*>(&oPlayerDispose));
    }
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

    for (const std::pair<std::string, std::shared_ptr<ISDK>> sdk : m_SDKs)
        sdk.second->OnJIT(address, size);
}

void MapleBase::Initialize()
{
    // todo: error handling

    m_Vanilla = std::make_shared<Vanilla>();
    m_Vanilla->Initialize(true);
    m_Vanilla->SetJITCallback(OnJIT);

    TryHookSetMousePosition();
    TryHookMouseViaKeyboardControls();
    TryHookScoreSubmit();
    TryHookOnPlayerLoadComplete();
    TryHookPlayerDispose();
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

std::shared_ptr<Vanilla> MapleBase::GetVanilla()
{
    return m_Vanilla;
}

void MapleBase::MakeScoreSubmissionUnsafe()
{
    m_ScoreSubmissionUnsafe = true;
}
