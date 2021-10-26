#pragma once

#include <windows.h>

#include <Explorer/TypeExplorer.h>

class AudioEngine
{
	static inline Field trackBassPlaybackRate;
	static inline Field trackVirtualPlaybackRate;
	static inline Field audioStreamField;
	static inline Field frequencyLockField;
	static inline Field trackVirtualPositionField;

	static inline void* currentTrackInstanceAddress = nullptr;
	static inline void* initialFrequencyAddress = nullptr;
	static inline void* nightcoreAddress = nullptr;
	static inline void* lastAudioTimeAccurateSetAddress = nullptr;
	static inline void* offsetAddress = nullptr;
	static inline void* extendedTimeAddress = nullptr;
	static inline void* timeAddress = nullptr;

	typedef BOOL(__stdcall* fnChannelSetAttribute)(DWORD handle, DWORD attrib, float value);
	static inline fnChannelSetAttribute channelSetAttribute = reinterpret_cast<fnChannelSetAttribute>(GetProcAddress(GetModuleHandleA("bass.dll"), "BASS_ChannelSetAttribute"));

	typedef double(__stdcall* fnChannelBytes2Seconds)(DWORD handle, DWORD64 pos);
	static inline fnChannelBytes2Seconds channelBytes2Seconds = reinterpret_cast<fnChannelBytes2Seconds>(GetProcAddress(GetModuleHandleA("bass.dll"), "BASS_ChannelBytes2Seconds"));
	
	typedef DWORD64(__stdcall* fnChannelGetPosition)(DWORD handle, DWORD mode);
	static inline fnChannelGetPosition channelGetPosition = reinterpret_cast<fnChannelGetPosition>(GetProcAddress(GetModuleHandleA("bass.dll"), "BASS_ChannelGetPosition"));
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
	static int Offset();
	static bool ExtendedTime();
	static int Time();
	static int TimeAccurate();
};