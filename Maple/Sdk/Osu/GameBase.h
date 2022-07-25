#pragma once

#include "Osu/OsuModes.h"

class GameBase
{
	static inline double tickrate = 1000.0 / 60.0;
public:
	static void Initialize();

	static OsuModes GetMode();
	static void SetTickrate(double value);
};
