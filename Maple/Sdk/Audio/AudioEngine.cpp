#include "AudioEngine.h"

#include "Vanilla.h"

#include "../Memory.h"
#include "../Mods/ModManager.h"
#include "../../Config/Config.h"
#include "../Player/Player.h"
#include "../../Features/Timewarp/Timewarp.h"
#include "../Osu/GameBase.h"

double __declspec(naked) AudioEngine::getCurrentPlaybackRateHook()
{
	__asm
	{
		mov[getCurrentPlaybackRateReturnAddress], esp
		jmp getCurrentPlaybackRateStub
	}
}

void __fastcall AudioEngine::setCurrentPlaybackRateHook(double rate)
{
	if (Config::Timewarp::Enabled && GameBase::GetMode() == OsuModes::Play && Player::GetInstance() && !Player::GetIsReplayMode())
	{
		rate = Timewarp::GetRate();

		GameBase::SetTickrate(1000.0 / 60.0 * (1.0 / Timewarp::GetRateMultiplier()));
	}
	else GameBase::SetTickrate(1000.0 / 60.0);

	oSetCurrentPlaybackRate(rate);
}

double __fastcall AudioEngine::getCurrentPlaybackRateStub()
{
	if (Vanilla::CheckAddressInModule(*static_cast<uintptr_t*>(getCurrentPlaybackRateReturnAddress), "clr.dll"))
		return ModManager::GetModPlaybackRate();

	return oGetCurrentPlaybackRate();
}

void AudioEngine::Initialize()
{
	Memory::AddObject("AudioEngine::Time", "7E 55 8B 76 10 DB 05", 0x7, 1);

	Memory::AddObject("AudioEngine::GetCurrentPlaybackRate", "55 8B EC 8B 0D ?? ?? ?? ?? 85 C9 75 08");
	Memory::AddHook("AudioEngine::GetCurrentPlaybackRate", "AudioEngine::GetCurrentPlaybackRate", reinterpret_cast<uintptr_t>(getCurrentPlaybackRateHook), reinterpret_cast<uintptr_t*>(&oGetCurrentPlaybackRate), VanillaHookType::UndetectedInline);

	Memory::AddObject("AudioEngine::SetCurrentPlaybackRate", "55 8B EC 56 8B 35 ?? ?? ?? ?? 85 F6 75 05 5E 5D C2 ?? ?? 33 D2 89 15");
	Memory::AddHook("AudioEngine::SetCurrentPlaybackRate", "AudioEngine::SetCurrentPlaybackRate", reinterpret_cast<uintptr_t>(setCurrentPlaybackRateHook), reinterpret_cast<uintptr_t*>(&oSetCurrentPlaybackRate), VanillaHookType::UndetectedInline);
}

int AudioEngine::GetTime()
{
	const uintptr_t timeAddress = Memory::Objects["AudioEngine::Time"];

	return timeAddress ? *reinterpret_cast<int*>(timeAddress) : 0;
}

bool AudioEngine::GetIsPaused()
{
	const uintptr_t timeAddress = Memory::Objects["AudioEngine::Time"];

	return timeAddress ? *reinterpret_cast<int*>(timeAddress + AUDIO_STATE_OFFSET) == 0 : false;
}
