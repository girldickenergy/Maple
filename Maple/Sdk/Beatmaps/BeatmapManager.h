#pragma once

#include <Explorer/TypeExplorer.h>
#include <COM/COMString.h>

class BeatmapManager
{
	typedef void*(__fastcall* fnGetBeatmapByFilename)(COMString* str);
	static inline fnGetBeatmapByFilename getBeatmapByFilename;
public:
	static inline TypeExplorer RawBeatmapManager;
	static void Initialize();

	static void* GetBeatmapByFilename(std::wstring filepath);
};