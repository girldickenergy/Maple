#pragma once

#include <Explorer/TypeExplorer.h>

enum class Mods
{
	None = 0,
	NoFail = 1,
	Easy = 2,
	TouchDevice = 4,
	Hidden = 8,
	HardRock = 16,
	SuddenDeath = 32,
	DoubleTime = 64,
	Relax = 128,
	HalfTime = 256,
	Nightcore = 512,
	Flashlight = 1024,
	Autoplay = 2048,
	SpunOut = 4096,
	Relax2 = 8192,
	Perfect = 16384,
	Key4 = 32768,
	Key5 = 65536,
	Key6 = 131072,
	Key7 = 262144,
	Key8 = 524288,
	FadeIn = 1048576,
	Random = 2097152,
	Cinema = 4194304,
	Target = 8388608,
	Key9 = 16777216,
	KeyCoop = 33554432,
	Key1 = 67108864,
	Key3 = 134217728,
	Key2 = 268435456,
	ScoreV2 = 536870912,
	Mirror = 1073741824,
	KeyMod = 521109504,
	FreeModAllowed = 1595913403,
	ScoreIncreaseMods = 1049688
};

inline Mods operator~(Mods a)
{
	return static_cast<Mods>(~static_cast<int>(a));
}

inline Mods operator&(Mods a, Mods b)
{
	return static_cast<Mods>(static_cast<int>(a) & static_cast<int>(b));
}

class ModManager
{
	static inline void* modStatusAddress = nullptr;
public:
	static inline TypeExplorer RawModManager;
	
	static void Initialize();
	static Mods CurrentMods();
	static bool IsModEnabled(Mods mod);
	static double ModPlaybackRate();
};