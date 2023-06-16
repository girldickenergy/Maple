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

		ReplayFrame InterpolateReplayFrames(ReplayFrame firstFrame, ReplayFrame secondFrame, int resyncTime);

		int GetIndexOfFrame(ReplayFrame replayFrame);
		ReplayFrame GetFrameAtIndex(int index);

		ReplayFrame GetFrameClosestToTime(int time);
		std::tuple<ReplayFrame, ReplayFrame> GetTwoClosestReplayFrames(int time);
		std::vector<ReplayFrame> GetReplayFramesWithinTimeFrame(int startTime, int endTime);

		bool DoesFrameExistOnTime(int time);

		Vector2 GetMousePositionAtTime(int time);

		Replay* GetReplay();
		std::string GetReplayBeatmapHash();
		PlayModes GetReplayPlayMode();
		Mods GetReplayMods();
	};
}