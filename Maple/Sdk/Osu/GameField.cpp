#include "GameField.h"

#include "xorstr.h"

void __fastcall GameField::TryFindInstance(uintptr_t start, unsigned int size)
{
    if (const uintptr_t instance = start && size
	? m_MapleBase->GetVanilla()->GetPatternScanner().FindPatternInRange(xorstr_("8B 15 ?? ?? ?? ?? 83 C2 04 8B 0D ?? ?? ?? ?? 39 09 FF 15 ?? ?? ?? ?? A1"), start, size)
	: m_MapleBase->GetVanilla()->GetPatternScanner().FindPattern(xorstr_("8B 15 ?? ?? ?? ?? 83 C2 04 8B 0D ?? ?? ?? ?? 39 09 FF 15 ?? ?? ?? ?? A1")))
    {
        m_Instance = *reinterpret_cast<GameFieldInternal***>(instance + 0xB);

        m_MapleBase->GetRuntimeLogger()->Log(LogLevel::Verbose, "Found GameBase.GameField");
    }
}

GameFieldInternal* GameField::GetInstance()
{
    return m_Instance ? *m_Instance : nullptr;
}

void __fastcall GameField::OnLoad(const std::shared_ptr<MapleBase>& mapleBase)
{
    ISDK::OnLoad(mapleBase);

    TryFindInstance();
}

void __fastcall GameField::OnJIT(uintptr_t address, unsigned size)
{
    if (!m_Instance)
        TryFindInstance(address, size);
}

std::string __fastcall GameField::GetName()
{
    return xorstr_("GameField");
}

Vector2 GameField::GetSize()
{
    const GameFieldInternal* instance = GetInstance();

    return instance ? instance->Size : Vector2();
}

float __fastcall GameField::GetRatio()
{
    const GameFieldInternal* instance = GetInstance();

    return instance ? instance->Size.Y / 384.f : 1.f;
}

Vector2 __fastcall GameField::GetOffset()
{
    const GameFieldInternal* instance = GetInstance();

    return instance ? instance->OffsetVector : Vector2(0.f);
}

Vector2 __fastcall GameField::DisplayToField(Vector2 display)
{
    return (display - GetOffset()) / GetRatio();
}

Vector2 __fastcall GameField::FieldToDisplay(Vector2 field)
{
    return field * GetRatio() + GetOffset();
}
