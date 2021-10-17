#pragma once

#include <windows.h>

#include <Explorer/TypeExplorer.h>

class AudioEngine
{
	static inline Field trackBassPlaybackRate;
	static inline Field trackVirtualPlaybackRate;
	static inline Field audioStreamField;
	
	static inline void* currentTrackInstanceAddress = nullptr;
	static inline void* initialFrequencyAddress = nullptr;
	static inline void* nightcoreAddress = nullptr;
public:
	static inline TypeExplorer RawAudioEngine;

	typedef void(__fastcall* fnSetCurrentPlaybackRate)(double rate);
	static inline fnSetCurrentPlaybackRate setCurrentPlaybackRate;
	
	static void Initialize();
	static void* CurrentTrackInstance();
	static int TrackHandle();
	static float InitialFrequency();
	static bool Nightcore();
	static double GetPlaybackRate();
	static void SetPlaybackRate(double rate);
};