#pragma once

#include <string>

#include "Math/Vector2.h"
#include "Osu/Keys/OsuKeys.h"
#include "Replays/Replay.h"

class ReplayBot
{
	static inline bool replayLoaded = false;
	static inline bool flipReplay = false;
	static inline int currentFrameIndex = 0;
	static inline Replay currentReplay;
	static inline Vector2 currentCursorPosition;
public:
	static inline bool Enabled = false;
	static inline bool DisableAiming = false;
	static inline bool DisableTapping = false;
	static inline bool Ready = false;

	static void Initialize();
	static void LoadReplay(std::string replayPath);
	static std::string GetReplayString();
	static void Update();
	static Vector2 GetCursorPosition();
	static OsuKeys GetCurrentKeys();
};
