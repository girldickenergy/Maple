#pragma once

#include "Osu/Keys/CatchTheBeatKeys.h"
#include "Osu/Keys/OsuKeys.h"
#include "Osu/Keys/OsuManiaKeys.h"
#include "Osu/Keys/TaikoKeys.h"

struct ReplayFrame
{
	float X;
	float Y;
	int TimeDiff;
	int Time;
	OsuKeys OsuKeys;
	TaikoKeys TaikoKeys;
	CatchTheBeatKeys CatchTheBeatKeys;
	OsuManiaKeys OsuManiaKeys;
};
