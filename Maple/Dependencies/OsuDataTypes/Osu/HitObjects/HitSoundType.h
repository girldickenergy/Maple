#pragma once

enum class HitSoundType
{
	None = 0,
	Normal = 1,
	Whistle = 2,
	Finish = 4,
	Clap = 8
};

inline HitSoundType operator&(HitSoundType a, HitSoundType b)
{
	return static_cast<HitSoundType>(static_cast<int>(a) & static_cast<int>(b));
}