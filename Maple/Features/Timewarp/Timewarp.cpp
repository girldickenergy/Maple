#include "Timewarp.h"

#include "../../SDK/Player/Player.h"
#include "../../Configuration/ConfigManager.h"
#include "../../SDK/Mods/ModManager.h"
#include "../../SDK/Player/Ruleset.h"

void Timewarp::Initialize()
{
    if (Player::GetPlayMode() == PlayModes::CatchTheBeat)
		Ruleset::SetCatcherSpeed(static_cast<float>(ConfigManager::CurrentConfig.Timewarp.Enabled ? GetRate() : ModManager::GetModPlaybackRate()) / 100.f);
}

double Timewarp::GetRate()
{
    const double rate = ConfigManager::CurrentConfig.Timewarp.RateLimitEnabled ? (std::min)(static_cast<double>(ConfigManager::CurrentConfig.Timewarp.Rate), ModManager::GetModPlaybackRate()) : static_cast<double>(ConfigManager::CurrentConfig.Timewarp.Rate);
    const double multiplier = ConfigManager::CurrentConfig.Timewarp.RateLimitEnabled ? (std::min)(static_cast<double>(ConfigManager::CurrentConfig.Timewarp.Multiplier) * ModManager::GetModPlaybackRate(), ModManager::GetModPlaybackRate()) : static_cast<double>(ConfigManager::CurrentConfig.Timewarp.Multiplier) * ModManager::GetModPlaybackRate();

    return ConfigManager::CurrentConfig.Timewarp.Type == 0 ? rate : multiplier;
}

double Timewarp::GetRateMultiplier()
{
    return GetRate() / ModManager::GetModPlaybackRate();
}
