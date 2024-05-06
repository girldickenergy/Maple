#pragma once

#include <array>

struct KeyState
{
    std::array<uint8_t, 256> KeyboardKeys;
    bool LeftMouseButtonDown;
    bool RightMouseButtonDown;

    KeyState(const std::array<uint8_t, 256>& keyboardKeys, bool leftMouseButtonDown, bool rightMouseButtonDown)
    {
        KeyboardKeys = keyboardKeys;
        LeftMouseButtonDown = leftMouseButtonDown;
        RightMouseButtonDown = rightMouseButtonDown;
    }
};