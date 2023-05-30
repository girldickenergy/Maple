#include "ReplayHandler.h"

ReplayEditor::ReplayHandler::ReplayHandler()
{
}

ReplayEditor::ReplayHandler::ReplayHandler(Replay selectedReplay, bool lazy)
{
	_selectedReplay = selectedReplay;

	if (lazy) return;

	removeStartFrames();
	removeFramesWithSameTime();
}

ReplayFrame ReplayEditor::ReplayHandler::GetFrameClosestToTime(int time)
{
	// This method is a performance critical component, make sure to monitor this performance closely
	int deviance = INT32_MAX;
	auto foundFrame = ReplayFrame();

	for (auto it = _selectedReplay.ReplayFrames.begin(); it != _selectedReplay.ReplayFrames.end(); ++it)
	{
		auto& frame = *it;
		if (std::abs(frame.Time - time) < deviance) {
			deviance = std::abs(frame.Time - time);
			foundFrame = frame;
		}
	}
	
	return foundFrame;
}

Vector2 ReplayEditor::ReplayHandler::GetMousePositionAtTime(int time)
{
	auto replayFrame = GetFrameClosestToTime(time);
	return Vector2(replayFrame.X, replayFrame.Y);
}

Replay* ReplayEditor::ReplayHandler::GetReplay()
{
	return &_selectedReplay;
}

std::string ReplayEditor::ReplayHandler::GetReplayBeatmapHash()
{
	return _selectedReplay.BeatmapHash;
}

PlayModes ReplayEditor::ReplayHandler::GetReplayPlayMode()
{
	return _selectedReplay.PlayMode;
}

Mods ReplayEditor::ReplayHandler::GetReplayMods()
{
	return _selectedReplay.Mods;
}

void ReplayEditor::ReplayHandler::removeStartFrames()
{
	_selectedReplay.ReplayFrames.erase(_selectedReplay.ReplayFrames.begin(), _selectedReplay.ReplayFrames.begin() + 2);
}

void ReplayEditor::ReplayHandler::removeFramesWithSameTime()
{
	auto framesToRemove = std::vector<int>();

	for (auto it = _selectedReplay.ReplayFrames.begin(); it != _selectedReplay.ReplayFrames.end(); ++it)
		if (std::distance(_selectedReplay.ReplayFrames.begin(), it) > 0)
			if ((*it).Time == (*(it - 1)).Time)
				framesToRemove.push_back(std::distance(_selectedReplay.ReplayFrames.begin(), it));

	for (auto const& index : framesToRemove)
		_selectedReplay.ReplayFrames.erase(_selectedReplay.ReplayFrames.begin() + index);
}