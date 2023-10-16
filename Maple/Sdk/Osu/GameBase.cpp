#include "GameBase.h"

#include "xorstr.h"

void GameBase::TryFindTime(uintptr_t start, unsigned int size)
{
    if (const uintptr_t time = start && size
        ? m_MapleBase->GetVanilla()->GetPatternScanner().FindPatternInRange(xorstr_("80 3D ?? ?? ?? ?? 00 74 1B A1 ?? ?? ?? ?? 2B 05 ?? ?? ?? ?? 3D"), start, size)
        : m_MapleBase->GetVanilla()->GetPatternScanner().FindPattern(xorstr_("80 3D ?? ?? ?? ?? 00 74 1B A1 ?? ?? ?? ?? 2B 05 ?? ?? ?? ?? 3D")))
    {
        m_Time = *reinterpret_cast<int**>(time + 0xA);

        m_MapleBase->GetRuntimeLogger()->Log(LogLevel::Verbose, "Found GameBase.Time");
    }
}

void GameBase::TryFindMode(uintptr_t start, unsigned int size)
{
    if (const uintptr_t mode = start && size
        ? m_MapleBase->GetVanilla()->GetPatternScanner().FindPatternInRange(xorstr_("80 B8 ?? ?? ?? ?? 00 75 19 A1 ?? ?? ?? ?? 83 F8 0B 74 0B"), start, size)
        : m_MapleBase->GetVanilla()->GetPatternScanner().FindPattern(xorstr_("80 B8 ?? ?? ?? ?? 00 75 19 A1 ?? ?? ?? ?? 83 F8 0B 74 0B")))
    {
        m_Mode = *reinterpret_cast<OsuModes**>(mode + 0xA);

        m_MapleBase->GetRuntimeLogger()->Log(LogLevel::Verbose, "Found GameBase.Mode");
    }
}

void GameBase::TryFindIsFullscreen(uintptr_t start, unsigned int size)
{
    if (const uintptr_t isFullscreen = start && size
        ? m_MapleBase->GetVanilla()->GetPatternScanner().FindPatternInRange(xorstr_("55 8B EC 57 56 53 8B F1 80 3D ?? ?? ?? ?? 00 75 05 E9 ?? ?? ?? ?? 81 3D ?? ?? ?? ?? ?? ?? ?? ?? 7C 06 80 7E"), start, size)
        : m_MapleBase->GetVanilla()->GetPatternScanner().FindPattern(xorstr_("55 8B EC 57 56 53 8B F1 80 3D ?? ?? ?? ?? 00 75 05 E9 ?? ?? ?? ?? 81 3D ?? ?? ?? ?? ?? ?? ?? ?? 7C 06 80 7E")))
    {
        m_IsFullscreen = *reinterpret_cast<bool**>(isFullscreen + 0xA);

        m_MapleBase->GetRuntimeLogger()->Log(LogLevel::Verbose, "Found GameBase.IsFullscreen");
    }
}

void GameBase::TryFindClientBounds(uintptr_t start, unsigned int size)
{
    if (const uintptr_t clientBounds = start && size
        ? m_MapleBase->GetVanilla()->GetPatternScanner().FindPatternInRange(xorstr_("56 FF 75 F0 8B 15 ?? ?? ?? ?? 83 C2 04 39 09"), start, size)
        : m_MapleBase->GetVanilla()->GetPatternScanner().FindPattern(xorstr_("56 FF 75 F0 8B 15 ?? ?? ?? ?? 83 C2 04 39 09")))
    {
        m_ClientBoundsAddress = *reinterpret_cast<uintptr_t*>(clientBounds + 0x6);

        m_MapleBase->GetRuntimeLogger()->Log(LogLevel::Verbose, "Found GameBase.ClientBounds");
    }
}

void GameBase::TryPatchTickrate(uintptr_t start, unsigned int size)
{
    if (const uintptr_t updateTimingPtr = start && size
        ? m_MapleBase->GetVanilla()->GetPatternScanner().FindPatternInRange(xorstr_("8B F1 8B 0D ?? ?? ?? ?? 33 D2 39 09 FF 15 ?? ?? ?? ?? 8B CE FF 15"), start, size)
        : m_MapleBase->GetVanilla()->GetPatternScanner().FindPattern(xorstr_("8B F1 8B 0D ?? ?? ?? ?? 33 D2 39 09 FF 15 ?? ?? ?? ?? 8B CE FF 15")))
    {
        m_FoundTickrate = true;

        const uintptr_t updateTiming = **reinterpret_cast<uintptr_t**>(updateTimingPtr + 0x16);

        if (const uintptr_t tickrate = m_MapleBase->GetVanilla()->GetPatternScanner().FindPatternInRange(xorstr_("DD 05 ?? ?? ?? ?? DC 25 ?? ?? ?? ?? D9 C0 DD 05"), updateTiming + 0x150, updateTiming + 0x1A0))
        {
            *reinterpret_cast<double**>(tickrate + 0x10) = &m_Tickrate;

            m_MapleBase->GetRuntimeLogger()->Log(LogLevel::Verbose, "Patched tickrate #1");
        }
        else
        {
            m_MapleBase->MakeScoreSubmissionUnsafe();

            m_MapleBase->GetRuntimeLogger()->Log(LogLevel::Important, "Failed to find tickrate #1! Invalidating score submission.");
        }

        if (const uintptr_t tickrate = m_MapleBase->GetVanilla()->GetPatternScanner().FindPatternInRange(xorstr_("1D ?? ?? ?? ?? DD 05 ?? ?? ?? ?? DC 25"), updateTiming + 0x1E0, updateTiming + 0x23F))
        {
            *reinterpret_cast<double**>(tickrate + 0xD) = &m_Tickrate;

            m_MapleBase->GetRuntimeLogger()->Log(LogLevel::Verbose, "Patched tickrate #2");
        }
        else
        {
            m_MapleBase->MakeScoreSubmissionUnsafe();

            m_MapleBase->GetRuntimeLogger()->Log(LogLevel::Important, "Failed to find tickrate #2! Invalidating score submission.");
        }

        if (const uintptr_t tickrate = m_MapleBase->GetVanilla()->GetPatternScanner().FindPatternInRange(xorstr_("DD 05 ?? ?? ?? ?? DD 05 ?? ?? ?? ?? DC 25"), updateTiming + 0x1E0, updateTiming + 0x23F))
        {
            *reinterpret_cast<double**>(tickrate + 0xE) = &m_Tickrate;

            m_MapleBase->GetRuntimeLogger()->Log(LogLevel::Verbose, "Patched tickrate #3");
        }
        else
        {
            m_MapleBase->MakeScoreSubmissionUnsafe();

            m_MapleBase->GetRuntimeLogger()->Log(LogLevel::Important, "Failed to find tickrate #3! Invalidating score submission.");
        }

        if (const uintptr_t tickrate = m_MapleBase->GetVanilla()->GetPatternScanner().FindPatternInRange(xorstr_("DD 1D ?? ?? ?? ?? DD 05 ?? ?? ?? ?? DC 35"), updateTiming + 0x220, updateTiming + 0x280))
        {
            *reinterpret_cast<double**>(tickrate + 0xE) = &m_Tickrate;

            m_MapleBase->GetRuntimeLogger()->Log(LogLevel::Verbose, "Patched tickrate #4");
        }
        else
        {
            m_MapleBase->MakeScoreSubmissionUnsafe();

            m_MapleBase->GetRuntimeLogger()->Log(LogLevel::Important, "Failed to find tickrate #4! Invalidating score submission.");
        }
    }
}

void __fastcall GameBase::OnLoad(const std::shared_ptr<MapleBase>& mapleBase)
{
    TryFindTime();
    TryFindMode();
    TryFindIsFullscreen();
    TryFindClientBounds();
    TryPatchTickrate();
}

void __fastcall GameBase::OnJIT(uintptr_t address, unsigned size)
{
    if (!m_Time)
        TryFindTime(address, size);

    if (!m_Mode)
        TryFindMode(address, size);

    if (!m_IsFullscreen)
        TryFindIsFullscreen(address, size);

    if (!m_ClientBoundsAddress)
        TryFindClientBounds(address, size);

    if (!m_FoundTickrate)
        TryPatchTickrate(address, size);
}

std::string __fastcall GameBase::GetName()
{
    return xorstr_("GameBase");
}

int GameBase::GetTime()
{
    return m_Time ? *m_Time : 0;
}

OsuModes GameBase::GetMode()
{
    return m_Mode ? *m_Mode : OsuModes::Menu;
}

bool GameBase::GetIsFullscreen()
{
    return m_IsFullscreen ? *m_IsFullscreen : true;
}

RectangleI GameBase::GetClientBounds()
{
    return m_ClientBoundsAddress ? *reinterpret_cast<RectangleI*>(*reinterpret_cast<uintptr_t*>(m_ClientBoundsAddress) + 0x4) : RectangleI();
}

void GameBase::SetTickrate(double value)
{
    m_Tickrate = value;
}
