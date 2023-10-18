#pragma once

#include "CLR/CLRObject.h"
#include "Numerics/Vector2.h"

struct GameFieldInternal : CLRObject
{
    CLRObject* Window;
    Vector2 Size;
    float ScaleFactor;
    bool CorrectionOffsetActive;
    Vector2 OffsetVector;
};
