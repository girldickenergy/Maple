#pragma once
#include "../../../Dependencies/Chiyo/Decoders/ReplayDecoder.h"
#include <Math/Vector2.h>

namespace ReplayEditor
{
	class ReplayHandler
	{
		Replay _selectedReplay;
	
		void removeStartFrames();
		void removeFramesWithSameTime();
	public:
		ReplayHandler();
		ReplayHandler(Replay selectedReplay, bool lazy = false);

		ReplayFrame GetFrameClosestToTime(int time);
		Vector2 GetMousePositionAtTime(int time);

		Replay* GetReplay();
		std::string GetReplayBeatmapHash();
		PlayModes GetReplayPlayMode();
		Mods GetReplayMods();
	};
}