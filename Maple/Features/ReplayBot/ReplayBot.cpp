#include "ReplayBot.h"

#define NOMINMAX
#include <cmath>
#include <algorithm>

#include "Decoders/ReplayDecoder.h"

#include "../../SDK/Mods/ModManager.h"
#include "../../SDK/Audio/AudioEngine.h"
#include "../../SDK/Osu/GameField.h"

void ReplayBot::Initialize()
{
	currentFrameIndex = 0;
	flipReplay = ModManager::CheckActive(Mods::HardRock) != ((currentReplay.Mods & Mods::HardRock) > Mods::None);
	Ready = Enabled && replayLoaded && !currentReplay.ReplayFrames.empty() && currentReplay.PlayMode == PlayModes::Osu;
}

void ReplayBot::LoadReplay(std::string replayPath)
{
	currentReplay = ReplayDecoder::Decode(replayPath);
	//erasing retarded peppy frames
	currentReplay.ReplayFrames.erase(currentReplay.ReplayFrames.begin(), currentReplay.ReplayFrames.size() > 2 ? currentReplay.ReplayFrames.begin() + 2 : currentReplay.ReplayFrames.end());
	replayLoaded = true;
}

void ReplayBot::LoadFromReplayEditor(Replay replay)
{
	currentReplay = replay;
	replayLoaded = true;
}

std::string ReplayBot::GetReplayString()
{
	if (!replayLoaded)
		return "none";

	std::string rulesetString;
	switch (currentReplay.PlayMode)
	{
	case PlayModes::Osu:
		rulesetString = "osu!standard";
		break;
	case PlayModes::Taiko:
		rulesetString = "osu!taiko";
		break;
	case PlayModes::CatchTheBeat:
		rulesetString = "osu!ctb";
		break;
	case PlayModes::OsuMania:
		rulesetString = "osu!mania";
		break;
	}

	return currentReplay.PlayerName + "'s " + rulesetString + " replay";
}

void ReplayBot::Update()
{
	bool newFrame = false;
	Vector2 pos;
	if (currentFrameIndex < currentReplay.ReplayFrames.size() - 1 && currentReplay.ReplayFrames[currentFrameIndex + 1].Time <= AudioEngine::GetTime())
	{
		currentFrameIndex++;
		newFrame = true;
	}

	bool runningSlow = AudioEngine::GetTime() - currentReplay.ReplayFrames[currentFrameIndex].Time > 16 && newFrame;

	//skip unnecessary frames
	if (runningSlow)
		while (currentFrameIndex < currentReplay.ReplayFrames.size() - 1 && currentReplay.ReplayFrames[currentFrameIndex].OsuKeys == OsuKeys::None && currentReplay.ReplayFrames[currentFrameIndex + 1].Time <= AudioEngine::GetTime())
			currentFrameIndex++;

	if (newFrame)
	{
		pos = Vector2(currentReplay.ReplayFrames[currentFrameIndex].X, currentReplay.ReplayFrames[currentFrameIndex].Y);
	}
	else if (currentReplay.ReplayFrames[currentFrameIndex].Time >= AudioEngine::GetTime())
	{
		int p1 = std::max(0, currentFrameIndex - 1);
		int p2 = currentFrameIndex;

		float t = currentReplay.ReplayFrames[p2].Time == currentReplay.ReplayFrames[p1].Time ? 0.f : std::max(0.f, (1 - (float)(currentReplay.ReplayFrames[p2].Time - AudioEngine::GetTime()) / (currentReplay.ReplayFrames[p2].Time - currentReplay.ReplayFrames[p1].Time)));
		pos = Vector2(std::lerp(currentReplay.ReplayFrames[p1].X, currentReplay.ReplayFrames[p2].X, t), std::lerp(currentReplay.ReplayFrames[p1].Y, currentReplay.ReplayFrames[p2].Y, t));
	}
	else
	{
		int p1 = currentFrameIndex;
		int p2 = std::min((int)currentReplay.ReplayFrames.size() - 1, currentFrameIndex + 1);

		float t = currentReplay.ReplayFrames[p2].Time == currentReplay.ReplayFrames[p1].Time ? 0.f : std::max(0.f, (1 - (float)(currentReplay.ReplayFrames[p2].Time - AudioEngine::GetTime()) / (currentReplay.ReplayFrames[p2].Time - currentReplay.ReplayFrames[p1].Time)));

		pos = Vector2(std::lerp(currentReplay.ReplayFrames[p1].X, currentReplay.ReplayFrames[p2].X, t), std::lerp(currentReplay.ReplayFrames[p1].Y, currentReplay.ReplayFrames[p2].Y, t));
	}

	currentCursorPosition = GameField::FieldToDisplay(flipReplay ? Vector2(pos.X, 384.f - pos.Y) : pos);
}

Vector2 ReplayBot::GetCursorPosition()
{
	return currentCursorPosition;
}

OsuKeys ReplayBot::GetCurrentKeys()
{
	return currentReplay.ReplayFrames[currentFrameIndex].OsuKeys;
}