#pragma once

#include "Vector2.h"

struct RectangleI
{
    int X = 0.f;
    int Y = 0.f;
    int Width = 0.f;
    int Height = 0.f;

    RectangleI() = default;

    RectangleI(int x, int y, int width, int height)
    {
        X = x;
        Y = y;
        Width = width;
        Height = height;
    }

    RectangleI(Vector2 position, Vector2 size)
    {
        X = position.X;
        Y = position.Y;
        Width = size.X;
        Height = size.Y;
    }

    [[nodiscard]] Vector2 Location() const
    {
        return { static_cast<float>(X), static_cast<float>(Y) };
    }

    [[nodiscard]] Vector2 Size() const
    {
        return { static_cast<float>(Width), static_cast<float>(Height) };
    }

    [[nodiscard]] int Left() const
    {
        return X;
    }

    [[nodiscard]] int Right() const
    {
        return X + Width;
    }

    [[nodiscard]] int Top() const
    {
        return Y;
    }

    [[nodiscard]] int Bottom() const
    {
        return Y + Height;
    }

    [[nodiscard]] Vector2 TopLeft() const
    {
        return { static_cast<float>(Left()), static_cast<float>(Top()) };
    }

    [[nodiscard]] Vector2 TopRight() const
    {
        return { static_cast<float>(Right()), static_cast<float>(Top()) };
    }

    [[nodiscard]] Vector2 BottomLeft() const
    {
        return { static_cast<float>(Left()), static_cast<float>(Bottom()) };
    }

    [[nodiscard]] Vector2 BottomRight() const
    {
        return { static_cast<float>(Right()), static_cast<float>(Bottom()) };
    }

    [[nodiscard]] Vector2 Center() const
    {
        return { static_cast<float>(X) + static_cast<float>(Width) / 2.f, static_cast<float>(Y) + static_cast<float>(Height) / 2.f };
    }
};