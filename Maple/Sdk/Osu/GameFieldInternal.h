#pragma once

#include <cstdint>

#include "Numerics/Vector2.h"

struct GameFieldInternal
{
    uintptr_t VTable;
    uintptr_t Window;
    float Width;
    float Height;
    float ScaleFactor;
    bool CorrectionOffsetActive;
    uint8_t _padding[3];
    Vector2 OffsetVector;
    Vector2 OffsetVectorWidescreen;
};
