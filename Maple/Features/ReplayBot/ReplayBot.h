#pragma once

#include <string>

#include "../../Dependencies/Chiyo/Replays/Replay.h"
#include "../../Sdk/DataTypes/Structs/Vector2.h"

class ReplayBot
{
	static inline bool replayLoaded = false;
	static inline bool flipReplay = false;
	static inline int currentFrameIndex = 0;
	static inline Replay currentReplay;
public:
	static inline bool Enabled = false;
	static inline bool DisableAiming = false;
	static inline bool DisableTapping = false;
	static inline bool Ready = false;

	static void Initialize();
	static void LoadReplay(std::string replayPath);
	static std::string GetReplayString();
	static Vector2 Update();
	static OsuKeys GetCurrentKeys();
};