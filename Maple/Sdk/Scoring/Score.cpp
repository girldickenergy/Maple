#include "Score.h"

#include "xorstr.h"

void __fastcall Score::TryFindInstance(uintptr_t start, unsigned int size)
{
    if (const uintptr_t instance = start && size
        ? m_MapleBase->GetVanilla()->GetPatternScanner().FindPatternInRange(xorstr_("00 7E ?? 8B 3D ?? ?? ?? ?? C6 87 ?? ?? ?? ?? 00 8B 3D ?? ?? ?? ?? 8B CF 39 09 E8 ?? ?? ?? ?? 8B 8E"), start, size)
        : m_MapleBase->GetVanilla()->GetPatternScanner().FindPattern(xorstr_("00 7E ?? 8B 3D ?? ?? ?? ?? C6 87 ?? ?? ?? ?? 00 8B 3D ?? ?? ?? ?? 8B CF 39 09 E8 ?? ?? ?? ?? 8B 8E")))
    {
        m_Instance = *reinterpret_cast<ScoreInternal***>(instance + 0x12);

        m_MapleBase->GetRuntimeLogger()->Log(LogLevel::Verbose, "Found Player.currentScore");
    }
}

ScoreInternal* Score::GetInstance()
{
    return m_Instance ? *m_Instance : nullptr;
}

void __fastcall Score::OnLoad(const std::shared_ptr<MapleBase>& mapleBase)
{
    TryFindInstance();
}

void __fastcall Score::OnJIT(uintptr_t address, unsigned size)
{
    if (!m_Instance)
        TryFindInstance(address, size);
}

std::string __fastcall Score::GetName()
{
    return xorstr_("Score");
}

int __fastcall Score::GetStartTime()
{
    const ScoreInternal* instance = GetInstance();

    return instance ? instance->StartTime : 0;
}

void __fastcall Score::SetStartTime(int startTime)
{
    ScoreInternal* instance = GetInstance();

    if (!instance)
    {
        m_MapleBase->MakeScoreSubmissionUnsafe();

        m_MapleBase->GetRuntimeLogger()->Log(LogLevel::Important, "Failed to set score start time because score instance was null! Invalidating score submission.");

        return;
    }

    instance->StartTime = startTime;
}
