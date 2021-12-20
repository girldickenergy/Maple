#pragma once

#include <math.h>

struct Vector2
{
	float X;
	float Y;
	bool IsNull = false;

	Vector2(float x, float y)
	{
		X = x;
		Y = y;
	}

	Vector2()
	{
		X = 0;
		Y = 0;
		IsNull = true;
	}

	Vector2 operator*(Vector2 v2)
	{
		return Vector2(X * v2.X, Y * v2.Y);
	}

	Vector2 operator+(Vector2 v2)
	{
		return Vector2(X + v2.X, Y + v2.Y);
	}

	Vector2 operator-(Vector2 v2)
	{
		return Vector2(X - v2.X, Y - v2.Y);
	}

	Vector2 operator*(float f2)
	{
		return Vector2(f2, f2) * (*this);
	}

	Vector2 operator*(int f2)
	{
		return Vector2(f2, f2) * (*this);
	}

	Vector2 operator+(int v2)
	{
		return Vector2(X + v2, Y + v2);
	}

	Vector2 operator /(float f2)
	{
		float invDiv = 1.0f / f2;
		return Vector2(
			X * invDiv,
			Y * invDiv);
	}

	Vector2 operator /(Vector2 v2)
	{
		return Vector2(X / v2.X, Y * v2.Y);
	}

	bool operator !=(Vector2 v2)
	{
		if (X != v2.X || Y != v2.Y)
			return true;
		return false;
	}

	float Length()
	{
		return sqrt(X * X + Y * Y);
	}

	float LengthSquared()
	{
		return X * X + Y * Y;
	}

	float Dot(Vector2 value2)
	{
		return X * value2.X + Y * value2.Y;
	}

	float Distance(Vector2 v2)
	{
		float dx = X - v2.X;
		float dy = Y - v2.Y;

		return sqrtf(dx * dx + dy * dy);
	}

	float Distance(Vector2* v2)
	{
		float dx = X - v2->X;
		float dy = Y - v2->Y;

		return sqrtf(dx * dx + dy * dy);
	}

	float DistanceSquared(Vector2 v2)
	{
		float dx = X - v2.X;
		float dy = Y - v2.Y;

		return dx * dx + dy * dy;
	}

	float DistanceSquared(Vector2* v2)
	{
		float dx = X - v2->X;
		float dy = Y - v2->Y;

		return dx * dx + dy * dy;
	}
};