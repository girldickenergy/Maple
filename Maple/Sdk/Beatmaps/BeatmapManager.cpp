#include "BeatmapManager.h"

#include <Vanilla.h>

void BeatmapManager::Initialize()
{
	RawBeatmapManager = Vanilla::Explorer["osu.GameplayElements.Beatmaps.BeatmapManager"];

	RawBeatmapManager["GetBeatmapByFilename"].Method.Compile();
	getBeatmapByFilename = static_cast<fnGetBeatmapByFilename>(RawBeatmapManager["GetBeatmapByFilename"].Method.GetNativeStart());
}

void* BeatmapManager::GetBeatmapByFilename(std::wstring filepath)
{
	COMString* comStr = COMString::CreateString(filepath.c_str());

	return getBeatmapByFilename(comStr);
}