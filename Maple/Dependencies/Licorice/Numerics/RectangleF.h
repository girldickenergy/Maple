#pragma once

#include "Vector2.h"

struct RectangleF
{
    float X = 0.f;
    float Y = 0.f;
    float Width = 0.f;
    float Height = 0.f;

    RectangleF() = default;

    RectangleF(float x, float y, float width, float height)
    {
        X = x;
        Y = y;
        Width = width;
        Height = height;
    }

    RectangleF(Vector2 position, Vector2 size)
    {
        X = position.X;
        Y = position.Y;
        Width = size.X;
        Height = size.Y;
    }

    [[nodiscard]] Vector2 Location() const
    {
        return {X, Y};
    }

    [[nodiscard]] Vector2 Size() const
    {
        return {Width, Height};
    }

    [[nodiscard]] float Left() const
    {
        return X;
    }

    [[nodiscard]] float Right() const
    {
        return X + Width;
    }

    [[nodiscard]] float Top() const
    {
        return Y;
    }

    [[nodiscard]] float Bottom() const
    {
        return Y + Height;
    }

    [[nodiscard]] Vector2 TopLeft() const
    {
        return { Left(), Top() };
    }

    [[nodiscard]] Vector2 TopRight() const
    {
        return { Right(), Top() };
    }

    [[nodiscard]] Vector2 BottomLeft() const
    {
        return { Left(), Bottom() };
    }

    [[nodiscard]] Vector2 BottomRight() const
    {
        return { Right(), Bottom() };
    }

    [[nodiscard]] Vector2 Center() const
    {
        return { X + Width / 2.f, Y + Height / 2.f };
    }
};