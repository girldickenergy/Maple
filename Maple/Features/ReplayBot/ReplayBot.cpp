#include "ReplayBot.h"

#define NOMINMAX
#include <cmath>

#include "../../Dependencies/Chiyo/Decoders/ReplayDecoder.h"
#include "../../Sdk/Player/Player.h"
#include "../../Sdk/Audio/AudioEngine.h"
#include "../../Sdk/Osu/GameField.h"
#include "../../Sdk/Mods/ModManager.h"

void ReplayBot::Initialize()
{
	currentFrameIndex = 2; //skipping retarded peppy frames
	flipReplay = ModManager::IsModEnabled(Mods::HardRock) != ((currentReplay.Mods & Mods::HardRock) > Mods::None);
	Ready = Enabled && replayLoaded && currentReplay.ReplayFrames.size() > 0 && currentReplay.PlayMode == PlayModes::Osu;
}

void ReplayBot::LoadReplay(std::string replayPath)
{
	currentReplay = ReplayDecoder::Decode(replayPath);
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

Vector2 ReplayBot::Update()
{
	bool newFrame = false;
	Vector2 pos;
	if (currentFrameIndex < currentReplay.ReplayFrames.size() - 1 && (currentReplay.ReplayFrames[currentFrameIndex + 1].Time <= AudioEngine::Time() || (currentReplay.ReplayFrames[currentFrameIndex].X == 256.f && currentReplay.ReplayFrames[currentFrameIndex].Y == -500.f)))
	{
		currentFrameIndex++;
		newFrame = true;
	}

	bool runningSlow = AudioEngine::Time() - currentReplay.ReplayFrames[currentFrameIndex].Time > 16 && newFrame;

	//skip unnecessary frames
	if (runningSlow)
		while (currentFrameIndex < currentReplay.ReplayFrames.size() - 1 && currentReplay.ReplayFrames[currentFrameIndex].OsuKeys == OsuKeys::None && currentReplay.ReplayFrames[currentFrameIndex + 1].Time <= AudioEngine::Time())
			currentFrameIndex++;

	if (newFrame)
	{
		pos = Vector2(currentReplay.ReplayFrames[currentFrameIndex].X, currentReplay.ReplayFrames[currentFrameIndex].Y);
	}
	else if (currentReplay.ReplayFrames[currentFrameIndex].Time >= AudioEngine::Time())
	{
		int p1 = std::max(0, currentFrameIndex - 1);
		int p2 = currentFrameIndex;

		float t = currentReplay.ReplayFrames[p2].Time == currentReplay.ReplayFrames[p1].Time ? 0.f : std::max(0.f, (1 - (float)(currentReplay.ReplayFrames[p2].Time - AudioEngine::Time()) / (currentReplay.ReplayFrames[p2].Time - currentReplay.ReplayFrames[p1].Time)));
		pos = Vector2(std::lerp(currentReplay.ReplayFrames[p1].X, currentReplay.ReplayFrames[p2].X, t), std::lerp(currentReplay.ReplayFrames[p1].Y, currentReplay.ReplayFrames[p2].Y, t));
	}
	else
	{
		int p1 = currentFrameIndex;
		int p2 = std::min((int)currentReplay.ReplayFrames.size() - 1, currentFrameIndex + 1);

		float t = currentReplay.ReplayFrames[p2].Time == currentReplay.ReplayFrames[p1].Time ? 0.f : std::max(0.f, (1 - (float)(currentReplay.ReplayFrames[p2].Time - AudioEngine::Time()) / (currentReplay.ReplayFrames[p2].Time - currentReplay.ReplayFrames[p1].Time)));

		pos = Vector2(std::lerp(currentReplay.ReplayFrames[p1].X, currentReplay.ReplayFrames[p2].X, t), std::lerp(currentReplay.ReplayFrames[p1].Y, currentReplay.ReplayFrames[p2].Y, t));
	}

	return GameField::FieldToDisplay(flipReplay ? Vector2(pos.X, 384.f - pos.Y) : pos);
}

OsuKeys ReplayBot::GetCurrentKeys()
{
	return currentReplay.ReplayFrames[currentFrameIndex].OsuKeys;
}
