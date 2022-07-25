#pragma once
#include <cstdint>

class StreamingManager
{
	typedef void(__fastcall* fnPushNewFrame)(uintptr_t frame);
	static inline fnPushNewFrame oPushNewFrame;
	static void __fastcall pushNewFrameHook(uintptr_t frame);

	typedef void(__fastcall* fnPurgeFrames)(int action, uintptr_t extra);
	static inline fnPurgeFrames oPurgeFrames;
	static void __fastcall purgeFramesHook(int action, uintptr_t extra);
public:
	static void Initialize();
};
