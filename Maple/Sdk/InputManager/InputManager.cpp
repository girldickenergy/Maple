#include "InputManager.h"

#include "xorstr.h"

void InputManager::TryFindCursorPosition()
{
    if (const uintptr_t smth = m_MapleBase->GetVanilla()->GetPatternScanner().FindPattern(xorstr_("55 8B EC 83 EC 14 A1 ?? ?? ?? ?? 83 C0 04 D9 45 08 D9 18 D9 45 0C D9 58 04 A1 ?? ?? ?? ?? 83 C0 04 D9 00 D9 5D FC")))
        cursorPositionAddress = *reinterpret_cast<uintptr_t*>(smth + 0x7);

    // todo: log this
}

void InputManager::TryFindCursorPositionSetters()
{
    if (uintptr_t applyHandler = m_MapleBase->GetVanilla()->GetPatternScanner().FindPattern(xorstr_("D9 C1 D9 1C 24 D9 5D ?? D9 5D ?? FF 15")))
    {
        applyHandler += 0xD;

        SetMousePosition = **reinterpret_cast<fnSetMousePosition**>(applyHandler);

        applyHandler += 0xF;

        while (*reinterpret_cast<uint16_t*>(applyHandler) != 0x15FF)
            applyHandler++;

        SetCursorHandlerPositions = **reinterpret_cast<fnSetCursorHanderPositions**>(applyHandler + 0x2);
    }

    // todo: log this
}

void __fastcall InputManager::OnLoad(const std::shared_ptr<MapleBase>& mapleBase)
{
    ISDK::OnLoad(mapleBase);

    TryFindCursorPosition();
    TryFindCursorPositionSetters();
}

void __fastcall InputManager::OnJIT(uintptr_t address, unsigned size)
{
    if (!cursorPositionAddress)
        TryFindCursorPosition();

    if (!SetCursorHandlerPositions || !SetMousePosition)
        TryFindCursorPositionSetters();
}

Vector2 __fastcall InputManager::GetCursorPosition()
{
    return cursorPositionAddress ? *reinterpret_cast<Vector2*>(*reinterpret_cast<uintptr_t*>(cursorPositionAddress) + 0x4) : Vector2(0.f, 0.f);
}

void __fastcall InputManager::SetCursorPosition(Vector2 position)
{
    if (!SetCursorHandlerPositions || !SetMousePosition)
        return;

    SetMousePosition(position);
    SetCursorHandlerPositions(position);
}
