#pragma once

#include "CLR/CLRObject.h"

struct GameModeInternal : CLRObject
{
    CLRObject* Game;
    CLRObject* Disposed;
    CLRObject* baseSpriteManager;
};
