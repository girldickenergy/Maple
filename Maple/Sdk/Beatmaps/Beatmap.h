#pragma once
#include <string>
#include <Windows.h>

class Beatmap
{
	static inline constexpr int BEATMAP_APPROACHRATE_OFFSET = 0x2C;
	static inline constexpr int BEATMAP_CIRCLESIZE_OFFSET = 0x30;
	static inline constexpr int BEATMAP_OVERALLDIFFICULTY_OFFSET = 0x38;
	static inline constexpr int BEATMAP_DISPLAYTITLEFULL_OFFSET = 0x80;

	uintptr_t _beatmapOsuPointer;

	float _approachRate;
	float _circleSize;
	float _overallDifficulty;
	std::string _displayTitleFull;
public:
	Beatmap();
	Beatmap(uintptr_t beatmapOsuPointer);

	void Update();
	float GetApproachRate();
	float GetCircleSize();
	float GetOverallDifficulty();
	uintptr_t GetBeatmapOsuPointer();
	std::string GetDisplayTitleFull();
};