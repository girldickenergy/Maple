#pragma once

#include <cstdint>

struct GameModeInternal
{
    uintptr_t Vtable;
    uintptr_t Game;
    uintptr_t Disposed;
    uintptr_t baseSpriteManager;
};
