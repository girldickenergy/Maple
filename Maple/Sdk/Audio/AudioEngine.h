#pragma once

#include <windows.h>

#include <Explorer/TypeExplorer.h>

class AudioEngine
{
	static inline Field trackBassPlaybackRate;
	static inline Field trackVirtualPlaybackRate;
	static inline Field audioStreamField;
	static inline Field frequencyLockField;
	
	static inline void* currentTrackInstanceAddress = nullptr;
	static inline void* initialFrequencyAddress = nullptr;
	static inline void* nightcoreAddress = nullptr;
	static inline void* lastAudioTimeAccurateSetAddress = nullptr;

	typedef BOOL(__stdcall* ChannelSetAttribute_t)(DWORD handle, DWORD attrib, float value);
	static inline ChannelSetAttribute_t channelSetAttribute = reinterpret_cast<ChannelSetAttribute_t>(GetProcAddress(GetModuleHandleA("bass.dll"), "BASS_ChannelSetAttribute"));
public:
	static inline TypeExplorer RawAudioEngine;
	static inline TypeExplorer RawAudioTrack;
	static inline TypeExplorer RawAudioTrackBass;
	static inline TypeExplorer RawAudioTrackVirtual;
	
	static void Initialize();
	static void* CurrentTrackInstance();
	static int TrackHandle();
	static float InitialFrequency();
	static bool Nightcore();
	static double GetPlaybackRate();
	static void SetPlaybackRate(double rate);
};