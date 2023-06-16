#include "ReplayHandler.h"
#include <cmath>
#include <algorithm>

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

ReplayFrame ReplayEditor::ReplayHandler::InterpolateReplayFrames(ReplayFrame firstFrame, ReplayFrame secondFrame, int resyncTime)
{
	// First check if resyncTime is within the two supplied ReplayFrames, and if not, return an empty ReplayFrame
	if (firstFrame.Time >= resyncTime || secondFrame.Time <= resyncTime)
		return ReplayFrame();

	// Calculate the linear interpolation factor
	int duration = secondFrame.Time - firstFrame.Time;
	float t = static_cast<float>(resyncTime - firstFrame.Time) / static_cast<float>(duration);
	t = std::clamp(t, 0.f, 1.f);

	auto interpolatedX = std::lerp(firstFrame.X, secondFrame.X, t);
	auto interpolatedY = std::lerp(firstFrame.Y, secondFrame.Y, t);
	auto interpolatedTime = std::lerp(firstFrame.Time, secondFrame.Time, t);

	auto interpolatedFrame = ReplayFrame();
	interpolatedFrame.X = interpolatedX;
	interpolatedFrame.Y = interpolatedY;
	interpolatedFrame.Time = interpolatedTime;

	return interpolatedFrame;
}

int ReplayEditor::ReplayHandler::GetIndexOfFrame(ReplayFrame replayFrame)
{
	for (auto it = _selectedReplay.ReplayFrames.begin(); it != _selectedReplay.ReplayFrames.end(); ++it)
	{
		auto& frame = *it;
		if (frame.X == replayFrame.X && frame.Y == replayFrame.Y && frame.Time == replayFrame.Time)
			return std::distance(_selectedReplay.ReplayFrames.begin(), it);
	}
	return -1;
}

ReplayFrame ReplayEditor::ReplayHandler::GetFrameAtIndex(int index)
{
	return _selectedReplay.ReplayFrames.at(index);
}

ReplayFrame ReplayEditor::ReplayHandler::GetFrameClosestToTime(int time)
{
	// This method is a performance critical component, make sure to monitor this performance closely
	int deviance = INT32_MAX;
	auto foundFrame = ReplayFrame();

	for (auto it = _selectedReplay.ReplayFrames.begin(); it != _selectedReplay.ReplayFrames.end(); ++it)
	{
		auto& frame = *it;
		if (std::abs(frame.Time - time) <= deviance) {
			deviance = std::abs(frame.Time - time);
			foundFrame = frame;
		}
	}

	return foundFrame;
}

std::tuple<ReplayFrame, ReplayFrame> ReplayEditor::ReplayHandler::GetTwoClosestReplayFrames(int time)
{
	auto closestFrame = GetFrameClosestToTime(time);

	ReplayFrame startFrame;
	ReplayFrame endFrame;

	// Check if the frame is before the time or after the time.
	// If the closest frame is infront of the time, we search for another frame after this time, and vice versa.
	if (time - closestFrame.Time > 0)
	{
		// Frame is infront time
		startFrame = closestFrame;

		// Now we grab the index of the frame and select the one that's after it.
		const auto frameIndex = GetIndexOfFrame(startFrame);
		endFrame = GetFrameAtIndex(frameIndex + 1);
	}
	else if (time - closestFrame.Time < 0)
	{
		// Frame is after time
		endFrame = closestFrame;

		// Now we grab the index of the frame and select the one that's before it.
		const auto frameIndex = GetIndexOfFrame(endFrame);
		startFrame = GetFrameAtIndex(frameIndex - 1);
	}
	else
	{
		// The time landed directly on a frame, we don't handle this here!
	}

	return { startFrame, endFrame };
}

std::vector<ReplayFrame> ReplayEditor::ReplayHandler::GetReplayFramesWithinTimeFrame(int startTime, int endTime)
{
	auto temporary = std::vector<ReplayFrame>();
	for (auto const& replayFrame : _selectedReplay.ReplayFrames)
	{
		if (replayFrame.Time >= startTime && replayFrame.Time <= endTime)
			temporary.push_back(replayFrame);
	}
	return temporary;
}

bool ReplayEditor::ReplayHandler::DoesFrameExistOnTime(int time)
{
	return std::ranges::any_of(_selectedReplay.ReplayFrames, [time](const auto& frame) { return frame.Time == time; });
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

	for (auto it = _selectedReplay.ReplayFrames.begin() + 1; it != _selectedReplay.ReplayFrames.end();)
	{
		if ((*it).Time == (*(it - 1)).Time)
			it = _selectedReplay.ReplayFrames.erase(it);
		else
			++it;
	}
}