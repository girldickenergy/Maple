#include "Timewarp.h"

#include "../../SDK/Player/Player.h"
#include "../../Config/Config.h"
#include "../../SDK/Mods/ModManager.h"
#include "../../SDK/Player/Ruleset.h"

void Timewarp::Initialize()
{
	if (Player::GetPlayMode() == PlayModes::CatchTheBeat)
		Ruleset::SetCatcherSpeed(static_cast<float>(Config::Timewarp::Enabled ? GetRate() : ModManager::GetModPlaybackRate()) / 100.f);
}

double Timewarp::GetRate()
{
	return Config::Timewarp::Type == 0 ? Config::Timewarp::Rate : ModManager::GetModPlaybackRate() * static_cast<double>(Config::Timewarp::Multiplier);
}

double Timewarp::GetRateMultiplier()
{
	return GetRate() / ModManager::GetModPlaybackRate();
}
