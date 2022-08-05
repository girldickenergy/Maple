#pragma once

#include <string>
#include <vector>

#include "Osu/Mods.h"
#include "Osu/PlayModes.h"

#include "LifeFrame.h"
#include "ReplayFrame.h"

struct Replay
{
	PlayModes PlayMode;
	int Version;
	std::string BeatmapHash;
	std::string PlayerName;
	std::string ReplayHash;
	unsigned short Count300;
	unsigned short Count100;
	unsigned short Count50;
	unsigned short CountGeki;
	unsigned short CountKatu;
	unsigned short CountMiss;
	int TotalScore;
	unsigned short MaxCombo;
	bool Perfect;
	Mods Mods;
	long long ReplayTimestamp;
	int ReplayLength;
	std::vector<LifeFrame> LifeFrames;
	std::vector<ReplayFrame> ReplayFrames;
	int Seed;
	long long OnlineId;
};
