#pragma once

#include "Numerics/Vector2.h"

struct CursorState
{
    Vector2 CursorPosition;
    bool ShouldResync;
    float ResyncStrength;

    CursorState(const Vector2& cursorPosition, bool shouldResync, bool resyncStrength)
    {
        CursorPosition = cursorPosition;
        ShouldResync = shouldResync;
        ResyncStrength = resyncStrength;
    }
};