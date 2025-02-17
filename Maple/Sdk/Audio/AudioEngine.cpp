#include "AudioEngine.h"

#include "VirtualizerSDK.h"
#include "xorstr.hpp"

#include "../Memory.h"
#include "../Mods/ModManager.h"
#include "../../Configuration/ConfigManager.h"
#include "../Player/Player.h"
#include "../../Features/Timewarp/Timewarp.h"
#include "../Osu/GameBase.h"
#include "../../Communication/Communication.h"

void __stdcall AudioEngine::setCurrentPlaybackRateHook(double rate)
{
	if (ConfigManager::CurrentConfig.Timewarp.Enabled && GameBase::GetMode() == OsuModes::Play && Player::GetInstance() && !Player::GetIsReplayMode())
	{
		rate = Timewarp::GetRate();

		GameBase::SetTickrate(1000.0 / 60.0 * (1.0 / Timewarp::GetRateMultiplier()));
	}
	else GameBase::SetTickrate(1000.0 / 60.0);

	[[clang::musttail]] return oSetCurrentPlaybackRate(rate);
}

void AudioEngine::Initialize()
{
	VIRTUALIZER_FISH_RED_START
	
	Memory::AddObject(xorstr_("AudioEngine::Time"), xorstr_("D9 58 2C 8B 3D ?? ?? ?? ?? 8B 1D"), 0xB, 1);

	Memory::AddObject(xorstr_("AudioEngine::SetCurrentPlaybackRate"), xorstr_("55 8B EC 56 8B 35 ?? ?? ?? ?? 85 F6 75 05 5E 5D C2 ?? ?? 33 D2 89 15 ?? ?? ?? ?? 80 3D ?? ?? ?? ?? 00 0F 94 C2 0F B6 D2 8B CE"));
	Memory::AddHook(xorstr_("AudioEngine::SetCurrentPlaybackRate"), xorstr_("AudioEngine::SetCurrentPlaybackRate"), reinterpret_cast<uintptr_t>(setCurrentPlaybackRateHook), reinterpret_cast<uintptr_t*>(&oSetCurrentPlaybackRate));

	VIRTUALIZER_FISH_RED_END
}

int AudioEngine::GetTime()
{
	const uintptr_t timeAddress = Memory::Objects[xorstr_("AudioEngine::Time")];

	return timeAddress ? *reinterpret_cast<int*>(timeAddress) : 0;
}

bool AudioEngine::GetIsPaused()
{
	const uintptr_t timeAddress = Memory::Objects[xorstr_("AudioEngine::Time")];

	return timeAddress ? *reinterpret_cast<int*>(timeAddress + AUDIO_STATE_OFFSET) == 0 : false;
}

double AudioEngine::GetModTempo()
{
	if (ModManager::CheckActive(Mods::HalfTime))
		return 0.75;

	if (ModManager::CheckActive(Mods::DoubleTime) && !ModManager::CheckActive(Mods::Nightcore))
		return 1.5;

	return 1.0;
}

float AudioEngine::GetModFrequency(float currentFrequency)
{
	if (ModManager::CheckActive(Mods::Nightcore) && ConfigManager::CurrentConfig.Timewarp.Enabled)
        return std::round(currentFrequency / (static_cast<float>(Timewarp::GetRate()) / 100.f) * 1.5f);

	return currentFrequency;
}
