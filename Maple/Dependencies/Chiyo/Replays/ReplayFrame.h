#pragma once

#include <Enums/Gameplay/OsuKeys.h>
#include <Enums/Gameplay/TaikoKeys.h>
#include <Enums/Gameplay/CatchTheBeatKeys.h>
#include <Enums/Gameplay/OsuManiaKeys.h>

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