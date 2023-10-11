#include "AudioEngine.h"

#include "xorstr.h"

void AudioEngine::TryFindTime(uintptr_t start, unsigned int size)
{
    if (const uintptr_t time = start && size
	? m_MapleBase->GetVanilla()->GetPatternScanner().FindPatternInRange(xorstr_("D9 58 2C 8B 3D ?? ?? ?? ?? 8B 1D"), start, size)
	: m_MapleBase->GetVanilla()->GetPatternScanner().FindPattern(xorstr_("D9 58 2C 8B 3D ?? ?? ?? ?? 8B 1D")))
    {
        m_Time = *reinterpret_cast<int**>(time + 0xB);
    }
}

void __fastcall AudioEngine::OnLoad(const std::shared_ptr<MapleBase>& mapleBase)
{
    TryFindTime();
}

void __fastcall AudioEngine::OnJIT(uintptr_t address, unsigned size)
{
    if (!m_Time)
        TryFindTime(address, size);
}

std::string __fastcall AudioEngine::GetName()
{
    return xorstr_("AudioEngine");
}

int AudioEngine::GetTime()
{
    return m_Time ? *m_Time : 0;
}

bool AudioEngine::GetIsPaused()
{
    return m_Time ? *reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(m_Time) + 0x30) == 0 : false;
}
