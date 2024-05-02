#include "ModManager.h"

#include "xorstr.h"

void ModManager::TryFindModStatus(uintptr_t start, unsigned int size)
{
    if (const uintptr_t modStatus = start && size
        ? m_MapleBase->GetVanilla()->GetPatternScanner().FindPatternInRange(xorstr_("53 8B F1 A1 ?? ?? ?? ?? 25 ?? ?? ?? ?? 85 C0 0F 9F C0 0F B6 C0 8B F8 80 3D"), start, size)
        : m_MapleBase->GetVanilla()->GetPatternScanner().FindPattern(xorstr_("53 8B F1 A1 ?? ?? ?? ?? 25 ?? ?? ?? ?? 85 C0 0F 9F C0 0F B6 C0 8B F8 80 3D")))
    {
        m_ModStatus = *reinterpret_cast<Mods**>(modStatus + 0x4);

        m_MapleBase->GetRuntimeLogger()->Log(LogLevel::Verbose, "Found ModManager.ModStatus");
    }
}

void __fastcall ModManager::OnLoad(const std::shared_ptr<MapleBase>& mapleBase)
{
    TryFindModStatus();
}

const char* __fastcall ModManager::GetName()
{
    return xorstr_("ModManager");
}

Mods __fastcall ModManager::GetActiveMods()
{
    return m_ModStatus ? *m_ModStatus : Mods::None;
}

bool __fastcall ModManager::CheckActive(Mods mods)
{
    return (GetActiveMods() & mods) > Mods::None;
}

double __fastcall ModManager::GetModPlaybackRate()
{
    if (CheckActive(Mods::HalfTime))
        return 75.;

    if (CheckActive(Mods::DoubleTime) || CheckActive(Mods::Nightcore))
        return 150.;

    return 100.;
}
