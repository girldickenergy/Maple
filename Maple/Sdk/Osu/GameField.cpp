#include "GameField.h"

#include "xorstr.h"

void __fastcall GameField::TryFindInternalInstance(uintptr_t start, unsigned int size)
{
    if (const uintptr_t instance = start && size
	    ? m_MapleBase->GetVanilla()->GetPatternScanner().FindPatternInRange(xorstr_("8B 15 ?? ?? ?? ?? 83 C2 04 8B 0D ?? ?? ?? ?? 39 09 FF 15 ?? ?? ?? ?? A1"), start, size)
	    : m_MapleBase->GetVanilla()->GetPatternScanner().FindPattern(xorstr_("8B 15 ?? ?? ?? ?? 83 C2 04 8B 0D ?? ?? ?? ?? 39 09 FF 15 ?? ?? ?? ?? A1")))
    {
        m_InternalInstanceAddress = *reinterpret_cast<uintptr_t*>(instance + 0xB);
    }

    // todo: log this
}

void __fastcall GameField::OnLoad(const std::shared_ptr<MapleBase>& mapleBase)
{
    ISDK::OnLoad(mapleBase);

    TryFindInternalInstance();
}

void __fastcall GameField::OnJIT(uintptr_t address, unsigned size)
{
    if (!m_InternalInstanceAddress)
        TryFindInternalInstance(address, size);
}

std::string __fastcall GameField::GetName()
{
    return xorstr_("GameField");
}

GameFieldInternal* GameField::GetInternalInstance()
{
    return m_InternalInstanceAddress ? *reinterpret_cast<GameFieldInternal**>(m_InternalInstanceAddress) : nullptr;
}

float __fastcall GameField::GetRatio()
{
    const GameFieldInternal* instance = GetInternalInstance();

    return instance ? instance->Height / 384.f : 1.f;
}

Vector2 __fastcall GameField::DisplayToField(Vector2 display)
{
    const GameFieldInternal* instance = GetInternalInstance();

    return (display - (instance ? instance->OffsetVector : Vector2(0.f, 0.f))) / GetRatio();
}

Vector2 __fastcall GameField::FieldToDisplay(Vector2 field)
{
    const GameFieldInternal* instance = GetInternalInstance();

    return field * GetRatio() + (instance ? instance->OffsetVector : Vector2(0.f, 0.f));
}
