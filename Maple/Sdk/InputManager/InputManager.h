#pragma once

#include "../ISDK.h"

struct Vector2
{
    float X = 0.f;
    float Y = 0.f;

    Vector2() = default;
    Vector2(float x, float y)
    {
        X = X;
        Y = Y;
    }
};

class InputManager : public ISDK
{
    typedef void(__fastcall* fnSetMousePosition)(Vector2 position);
    typedef void(__fastcall* fnSetCursorHanderPositions)(Vector2 position);

    fnSetMousePosition SetMousePosition = nullptr;
    fnSetCursorHanderPositions SetCursorHandlerPositions = nullptr;

    uintptr_t cursorPositionAddress = 0u;

    void TryFindCursorPosition();
    void TryFindCursorPositionSetters();

public:
    void __fastcall OnLoad(const std::shared_ptr<MapleBase>& mapleBase) override;
    void __fastcall OnJIT(uintptr_t address, unsigned size) override;

    Vector2 __fastcall GetCursorPosition();
    void __fastcall SetCursorPosition(Vector2 position);
};
