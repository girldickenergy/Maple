#pragma once
#include <Explorer/TypeExplorer.h>
#include <Vanilla.h>
#include "COM/COMString.h"
#include <iostream>
#include <format>

class Beatmap
{
	void* beatmapOsuPointer;

	Field difficultyApproachRateField;
	Field difficultyCircleSizeField;
	Field difficultyOverallField;
	Field displayTitleFullField;

	float approachRate;
	float circleSize;
	float overallDifficulty;
	std::wstring_view displayTitleFull;
public:
	TypeExplorer RawBeatmapBase;
	TypeExplorer RawBeatmap;
	Beatmap(void* _beatmapOsuPointer);

	void Update();
	float GetApproachRate();
	float GetCircleSize();
	float GetOverallDifficulty();
	void* GetBeatmapOsuPointer();
	std::wstring GetDisplayTitleFull();
};