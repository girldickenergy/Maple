#include "AudioEngine.h"

#include <intrin.h>

#include "ThemidaSDK.h"
#include "Vanilla.h"

#include "../Memory.h"
#include "../Mods/ModManager.h"
#include "../../Config/Config.h"
#include "../Player/Player.h"
#include "../../Features/Timewarp/Timewarp.h"
#include "../Osu/GameBase.h"
#include "../../Utilities/Security/xorstr.hpp"

double __fastcall AudioEngine::getCurrentPlaybackRateHook(uintptr_t ecx, uintptr_t edx)
{
	const uintptr_t returnAddress = reinterpret_cast<uintptr_t>(_ReturnAddress());
	if (Vanilla::CheckAddressInModule(returnAddress, "clr.dll"))
		return ModManager::GetModPlaybackRate();

	return oGetCurrentPlaybackRate(ecx, edx);
}

void __stdcall AudioEngine::setCurrentPlaybackRateHook(double rate)
{
	std::unique_lock lock(mutex);

	if (Config::Timewarp::Enabled && GameBase::GetMode() == OsuModes::Play && Player::GetInstance() && !Player::GetIsReplayMode())
	{
		rate = Timewarp::GetRate();

		GameBase::SetTickrate(1000.0 / 60.0 * (1.0 / Timewarp::GetRateMultiplier()));
	}
	else GameBase::SetTickrate(1000.0 / 60.0);

	oSetCurrentPlaybackRate(rate);
}

void AudioEngine::Initialize()
{
	STR_ENCRYPT_START

	Memory::AddObject(xor ("AudioEngine::Time"), xor ("7E 55 8B 76 10 DB 05"), 0x7, 1);

	Memory::AddObject(xor ("AudioEngine::GetCurrentPlaybackRate"), xor ("55 8B EC 8B 0D ?? ?? ?? ?? 85 C9 75 08"));
	Memory::AddHook(xor ("AudioEngine::GetCurrentPlaybackRate"), xor ("AudioEngine::GetCurrentPlaybackRate"), reinterpret_cast<uintptr_t>(getCurrentPlaybackRateHook), reinterpret_cast<uintptr_t*>(&oGetCurrentPlaybackRate), VanillaHookType::UndetectedInline);

	Memory::AddObject(xor ("AudioEngine::SetCurrentPlaybackRate"), xor ("55 8B EC 56 8B 35 ?? ?? ?? ?? 85 F6 75 05 5E 5D C2 ?? ?? 33 D2 89 15"));
	Memory::AddHook(xor ("AudioEngine::SetCurrentPlaybackRate"), xor ("AudioEngine::SetCurrentPlaybackRate"), reinterpret_cast<uintptr_t>(setCurrentPlaybackRateHook), reinterpret_cast<uintptr_t*>(&oSetCurrentPlaybackRate), VanillaHookType::UndetectedInline);

	STR_ENCRYPT_END
}

int AudioEngine::GetTime()
{
	const uintptr_t timeAddress = Memory::Objects[xor ("AudioEngine::Time")];

	return timeAddress ? *reinterpret_cast<int*>(timeAddress) : 0;
}

bool AudioEngine::GetIsPaused()
{
	const uintptr_t timeAddress = Memory::Objects[xor ("AudioEngine::Time")];

	return timeAddress ? *reinterpret_cast<int*>(timeAddress + AUDIO_STATE_OFFSET) == 0 : false;
}
