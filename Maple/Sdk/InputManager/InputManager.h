#pragma once

#include "../ISDK.h"
#include "../Vector2.h"

class InputManager : public ISDK
{
    typedef void(__fastcall* fnSetMousePosition)(Vector2 position);
    typedef void(__fastcall* fnSetCursorHanderPositions)(Vector2 position);

    fnSetMousePosition SetMousePosition = nullptr;
    fnSetCursorHanderPositions SetCursorHandlerPositions = nullptr;

    uintptr_t cursorPositionAddress = 0u;

    void TryFindCursorPosition(uintptr_t start = 0u, unsigned int size = 0);
    void TryFindCursorPositionSetters(uintptr_t start = 0u, unsigned int size = 0);

public:
    void __fastcall OnLoad(const std::shared_ptr<MapleBase>& mapleBase) override;
    void __fastcall OnJIT(uintptr_t address, unsigned size) override;

    Vector2 __fastcall GetCursorPosition();
    void __fastcall SetCursorPosition(Vector2 position);
};
