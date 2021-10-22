#pragma once

enum class HitObjectType
{
	None = 0,
	Normal = 1,
	Slider = 2,
	NewCombo = 4,
	NormalNewCombo = 5,
	SliderNewCombo = 6,
	Spinner = 8,
	ColourHax = 112,
	Hold = 128,
	ManiaLong = 128
};

inline HitObjectType operator&(HitObjectType a, HitObjectType b)
{
	return static_cast<HitObjectType>(static_cast<int>(a) & static_cast<int>(b));
}
