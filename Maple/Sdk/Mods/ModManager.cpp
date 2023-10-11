#include "ModManager.h"

#include "xorstr.h"

void ModManager::TryFindModStatus(uintptr_t start, unsigned int size)
{
    if (const uintptr_t modStatus = start && size
        ? m_MapleBase->GetVanilla()->GetPatternScanner().FindPatternInRange(xorstr_("53 8B F1 A1 ?? ?? ?? ?? 25 ?? ?? ?? ?? 85 C0 0F 9F C0 0F B6 C0 8B F8 80 3D"), start, size)
        : m_MapleBase->GetVanilla()->GetPatternScanner().FindPattern(xorstr_("53 8B F1 A1 ?? ?? ?? ?? 25 ?? ?? ?? ?? 85 C0 0F 9F C0 0F B6 C0 8B F8 80 3D")))
    {
        m_ModStatus = *reinterpret_cast<OsuMods**>(modStatus + 0x4);
    }
}

void __fastcall ModManager::OnLoad(const std::shared_ptr<MapleBase>& mapleBase)
{
    TryFindModStatus();
}

void __fastcall ModManager::OnJIT(uintptr_t address, unsigned size)
{
    if (!m_ModStatus)
        TryFindModStatus(address, size);
}

std::string __fastcall ModManager::GetName()
{
    return xorstr_("ModManager");
}

OsuMods __fastcall ModManager::GetActiveMods()
{
    return m_ModStatus ? *m_ModStatus : OsuMods::None;
}

bool __fastcall ModManager::CheckActive(OsuMods mods)
{
    return (GetActiveMods() & mods) > OsuMods::None;
}

double __fastcall ModManager::GetModPlaybackRate()
{
    if (CheckActive(OsuMods::HalfTime))
        return 75.;

    if (CheckActive(OsuMods::DoubleTime) || CheckActive(OsuMods::Nightcore))
        return 150.;

    return 100.;
}
