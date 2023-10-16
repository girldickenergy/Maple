#pragma once

#include <cmath>

struct Vector2
{
    float X = 0.f;
    float Y = 0.f;

    Vector2() = default;

    Vector2(float x, float y)
    {
        X = x;
        Y = y;
    }

    Vector2(float val)
    {
        X = val;
        Y = val;
    }

    [[nodiscard]] float Length() const
    {
        return std::sqrt(X * X + Y * Y);
    }

    [[nodiscard]] float LengthSquared() const
    {
        return X * X + Y * Y;
    }

    [[nodiscard]] float Dot(Vector2 v2) const
    {
        return X * v2.X + Y * v2.Y;
    }

    [[nodiscard]] float Distance(Vector2 v2) const
    {
        const float dx = X - v2.X;
        const float dy = Y - v2.Y;

        return sqrtf(dx * dx + dy * dy);
    }

    [[nodiscard]] float DistanceSquared(Vector2 v2) const
    {
        const float dx = X - v2.X;
        const float dy = Y - v2.Y;

        return dx * dx + dy * dy;
    }

    Vector2& operator=(const Vector2& v2)
    {
        if (this != &v2)
        {
            X = v2.X;
            Y = v2.Y;
        }

        return *this;
    }

    Vector2 operator+(const Vector2& v2) const
    {
        return {X + v2.X, Y + v2.Y};
    }

    Vector2& operator+=(const Vector2& v2)
    {
        X += v2.X;
        Y += v2.Y;

        return *this;
    }

    Vector2 operator-(const Vector2& v2) const
    {
        return {X - v2.X, Y - v2.Y};
    }

    Vector2& operator-=(const Vector2& v2)
    {
        X -= v2.X;
        Y -= v2.Y;

        return *this;
    }

    Vector2 operator*(const Vector2& v2) const
    {
        return {X * v2.X, Y * v2.Y};
    }

    Vector2& operator*=(const Vector2& v2)
    {
        X *= v2.X;
        Y *= v2.Y;

        return *this;
    }

    Vector2 operator/(const Vector2& v2) const
    {
        return {X / v2.X, Y / v2.Y};
    }

    Vector2& operator/=(const Vector2& v2)
    {
        X /= v2.X;
        X /= v2.X;

        return *this;
    }

    bool operator==(const Vector2& v2) const
    {
        constexpr float epsilon = 0.00001f;

        return std::abs(X - v2.X) < epsilon && std::abs(Y - v2.Y) < epsilon;
    }

    bool operator!=(const Vector2& v2) const
    {
        return !(*this == v2);
    }
};