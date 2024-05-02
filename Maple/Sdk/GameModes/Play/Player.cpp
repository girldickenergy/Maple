#include "Player.h"

#include "xorstr.h"

void __fastcall Player::TryFindInstance(uintptr_t start, unsigned int size)
{
    if (const uintptr_t instance = start && size
	? m_MapleBase->GetVanilla()->GetPatternScanner().FindPatternInRange(xorstr_("80 3D ?? ?? ?? ?? 00 75 26 A1 ?? ?? ?? ?? 85 C0 74 0C"), start, size)
	: m_MapleBase->GetVanilla()->GetPatternScanner().FindPattern(xorstr_("80 3D ?? ?? ?? ?? 00 75 26 A1 ?? ?? ?? ?? 85 C0 74 0C")))
    {
        m_Instance = *reinterpret_cast<PlayerInternal***>(instance + 0xA);

        m_MapleBase->GetRuntimeLogger()->Log(LogLevel::Verbose, "Found Player.Instance");
    }
}

void __fastcall Player::TryFindIsRetrying(uintptr_t start, unsigned int size)
{
    if (const uintptr_t isRetrying = start && size
	? m_MapleBase->GetVanilla()->GetPatternScanner().FindPatternInRange(xorstr_("8B CE FF 15 ?? ?? ?? ?? C6 05 ?? ?? ?? ?? 00"), start, size)
	: m_MapleBase->GetVanilla()->GetPatternScanner().FindPattern(xorstr_("8B CE FF 15 ?? ?? ?? ?? C6 05 ?? ?? ?? ?? 00")))
    {
        m_IsRetrying = *reinterpret_cast<bool**>(isRetrying + 0xA);

        m_MapleBase->GetRuntimeLogger()->Log(LogLevel::Verbose, "Found Player.Retrying");
    }
}

void __fastcall Player::TryFindIsFailed(uintptr_t start, unsigned int size)
{
    if (const uintptr_t isFailed = start && size
	? m_MapleBase->GetVanilla()->GetPatternScanner().FindPatternInRange(xorstr_("8B 15 ?? ?? ?? ?? 89 90 ?? ?? ?? ?? 80 3D ?? ?? ?? ?? 00 74 57 80 3D"), start, size)
	: m_MapleBase->GetVanilla()->GetPatternScanner().FindPattern(xorstr_("8B 15 ?? ?? ?? ?? 89 90 ?? ?? ?? ?? 80 3D ?? ?? ?? ?? 00 74 57 80 3D")))
    {
        m_IsFailed = *reinterpret_cast<bool**>(isFailed + 0xE);

        m_MapleBase->GetRuntimeLogger()->Log(LogLevel::Verbose, "Found Player.Failed");
    }
}

PlayerInternal* Player::GetInstance()
{
    return m_Instance ? *m_Instance : nullptr;
}

void __fastcall Player::OnLoad(const std::shared_ptr<MapleBase>& mapleBase)
{
    TryFindInstance();
    TryFindIsRetrying();
    TryFindIsFailed();
}

const char* __fastcall Player::GetName()
{
    return xorstr_("Player");
}

bool Player::GetIsLoaded()
{
    const PlayerInternal* instance = GetInstance();

    return instance && instance->AsyncLoadComplete;
}

bool Player::GetIsReplayMode()
{
    const PlayerInternal* instance = GetInstance();

    return instance ? instance->replayModeStable : false;
}

PlayModes Player::GetPlayMode()
{
    const PlayerInternal* instance = GetInstance();

    return instance ? instance->ModeOriginal : PlayModes::Osu;
}

bool Player::GetIsRetrying()
{
    return m_IsRetrying ? *m_IsRetrying : false;
}

bool Player::GetIsFailed()
{
    return m_IsFailed ? *m_IsFailed : false;
}
