#pragma once

class SpectateHandler
{
	typedef void(__fastcall* fnPushNewFrame)(void* bReplayFrame_f);
	typedef void(__fastcall* fnPurgeFrames)(int action, void* extra);
public:
	static inline fnPushNewFrame oPushNewFrame;
	static inline fnPurgeFrames oPurgeFrames;
	static void __fastcall PushNewFrameHook(void* bReplayFrame_f);
	static void __fastcall PurgeFramesHook(int action, void* extra);
};