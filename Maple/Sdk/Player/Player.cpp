#include "Player.h"

#include "VirtualizerSDK.h"
#include "xorstr.hpp"

#include "HitObjectManager.h"
#include "../Memory.h"
#include "../Osu/GameBase.h"
#include "../../Features/Timewarp/Timewarp.h"
#include "../../Features/ReplayBot/ReplayBot.h"
#include "../../Features/Enlighten/Enlighten.h"
#include "../../UI/Windows/MainMenu.h"
#include "../../Features/AimAssist/AimAssist.h"
#include "../../Features/Relax/Relax.h"
#include "../../Configuration/ConfigManager.h"
#include "../../Logging/Logger.h"
#include "../../Features/TaikoMania/TaikoMania.h"

int __fastcall Player::onLoadCompleteHook(uintptr_t instance, bool success)
{
	if (success)
	{
		MainMenu::Hide();

		HitObjectManager::RestoreVisuals();
		HitObjectManager::CacheHitObjects();

		Enlighten::Initialize();
		Timewarp::Initialize();
		Relax::Initialize();
		AimAssist::Initialize();
		ReplayBot::Initialize();
		TaikoMania::Initialize();
	}

	[[clang::musttail]] return oOnLoadComplete(instance, success);
}

void __fastcall Player::updateFlashlightHook(uintptr_t instance)
{
	if (!ConfigManager::CurrentConfig.Visuals.Removers.FlashlightRemoverEnabled || GameBase::GetMode() != OsuModes::Play || GetIsReplayMode())
		[[clang::musttail]] return oUpdateFlashlight(instance);
}

void __fastcall Player::handleScoreSubmissionHook(uintptr_t instance)
{
	if (GetAnticheatFlag() != 0)
		Logger::Log(LogSeverity::Warning, xorstr_("AC flag is not zero! Flag -> %d"), GetAnticheatFlag());

	ResetAnticheatFlag();

	if (ConfigManager::CurrentConfig.Misc.ScoreSubmissionType == 1 || ConfigManager::ForceDisableScoreSubmission)
		return;

	[[clang::musttail]] return oHandleScoreSubmission(instance);
}

void Player::Initialize()
{
	VIRTUALIZER_FISH_RED_START
	
	Memory::AddObject(xorstr_("Player::Instance"), xorstr_("80 3D ?? ?? ?? ?? 00 75 26 A1 ?? ?? ?? ?? 85 C0 74 0C"), 0xA, 1);
	Memory::AddObject(xorstr_("Player::Mode"), xorstr_("A1 ?? ?? ?? ?? A3 ?? ?? ?? ?? A1 ?? ?? ?? ?? 89 86"), 0xB, 1);
	Memory::AddObject(xorstr_("Player::Retrying"), xorstr_("8B CE FF 15 ?? ?? ?? ?? C6 05 ?? ?? ?? ?? 00"), 0xA, 1);
	Memory::AddObject(xorstr_("Player::Failed"), xorstr_("8B 15 ?? ?? ?? ?? 89 90 ?? ?? ?? ?? 80 3D ?? ?? ?? ?? 00 74 57 80 3D"), 0xE, 1);
	Memory::AddObject(xorstr_("Player::Flag"), xorstr_("C7 86 ?? ?? ?? ?? ?? ?? ?? ?? F7 05"), 0xC, 1);

	Memory::AddObject(xorstr_("Player::GetAllowSubmissionVariableConditions"), xorstr_("55 8B EC 56 8B F1 A1 ?? ?? ?? ?? 2B 86"));
	Memory::AddPatch(xorstr_("Player::GetAllowSubmissionVariableConditions_HackCheck"), xorstr_("Player::GetAllowSubmissionVariableConditions"), xorstr_("83 BE ?? ?? ?? ?? 00 7E 1C"), 0x80, 0x0, {0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90});

	Memory::AddObject(xorstr_("Player::HandleScoreSubmission"), xorstr_("55 8B EC 57 56 53 ?? ?? ?? 8B F1 80 BE ?? ?? ?? ?? 00 75 26"));
	Memory::AddPatch(xorstr_("Player::HandleScoreSubmission_HackCheck"), xorstr_("Player::HandleScoreSubmission"), xorstr_("80 78 7C 00 0F 84"), 0x40F, 0x5, { 0x8D });
	Memory::AddHook(xorstr_("Player::HandleScoreSubmission"), xorstr_("Player::HandleScoreSubmission"), reinterpret_cast<uintptr_t>(handleScoreSubmissionHook), reinterpret_cast<uintptr_t*>(&oHandleScoreSubmission));

	Memory::AddObject(xorstr_("Player::Update"), xorstr_("55 8B EC 57 56 53 81 EC ?? ?? ?? ?? 8B F1 8D BD ?? ?? ?? ?? B9 ?? ?? ?? ?? 33 C0 F3 AB 8B CE 89 8D ?? ?? ?? ?? 8B 8D ?? ?? ?? ?? FF 15 ?? ?? ?? ?? 8B 8D ?? ?? ?? ?? FF 15 ?? ?? ?? ?? 85 C0 74 05"));
	Memory::AddPatch(xorstr_("Player::Update_AudioCheck"), xorstr_("Player::Update"), xorstr_("0F 85 ?? ?? ?? ?? 83 BE ?? ?? ?? ?? FF"), 0x1BCC, 0x1, { 0x8D });

	Memory::AddObject(xorstr_("Player::CheckFlashlightHax"), xorstr_("55 8B EC 57 56 53 83 EC 18 8B F9 80 3D"));
	Memory::AddPatch(xorstr_("Player::CheckFlashlightHax_FLCheck"), xorstr_("Player::CheckFlashlightHax"), xorstr_("80 3D ?? ?? ?? ?? 00 75 24 83 BF"), 0x375, 0x7, { 0x7D });

	Memory::AddObject(xorstr_("Player::HaxCheckMouse"), xorstr_("55 8B EC 57 56 83 EC 54 80 3D"));
	Memory::AddPatch(xorstr_("Player::HaxCheckMouse_MouseCheck"), xorstr_("Player::HaxCheckMouse"), xorstr_("80 3D ?? ?? ?? ?? 00 75 14"), 0x2C0, 0x7, { 0x7D });

	Memory::AddObject(xorstr_("Player::CheckAimAssist"), xorstr_("55 8B EC 57 56 83 EC 24 8B F1 8D 86"));
	Memory::AddPatch(xorstr_("Player::CheckAimAssist_AACheck"), xorstr_("Player::CheckAimAssist"), xorstr_("80 38 00 74 0B"), 0x212, 0x3, { 0x7D });

	Memory::AddObject(xorstr_("Player::HaxCheckPass"), xorstr_("55 8B EC 57 56 53 83 EC 2C 8B F1 8D 7D C8 B9 ?? ?? ?? ?? 33 C0 F3 AB 8B CE 83 3D ?? ?? ?? ?? 00 75 0A B8"));
	Memory::AddPatch(xorstr_("Player::HaxCheckPass_UniqueIDCheck"), xorstr_("Player::HaxCheckPass"), xorstr_("EB 02 33 C0 85 C0 75 13"), 0x400, 0x8, { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 });

	Memory::AddObject(xorstr_("Player::OnLoadComplete"), xorstr_("55 8B EC 57 56 53 83 EC 54 8B F1 8D 7D AC B9 ?? ?? ?? ?? 33 C0 F3 AB 8B CE 89 4D B0 33 C9 89 0D"));
	Memory::AddHook(xorstr_("Player::OnLoadComplete"), xorstr_("Player::OnLoadComplete"), reinterpret_cast<uintptr_t>(onLoadCompleteHook), reinterpret_cast<uintptr_t*>(&oOnLoadComplete));

	Memory::AddObject(xorstr_("Player::UpdateFlashlight"), xorstr_("55 8B EC 57 56 8B F1 83 BE ?? ?? ?? ?? 00 74 32 83 7E 68 00 74 2C A1 ?? ?? ?? ?? 8B 50 1C"));
	Memory::AddHook(xorstr_("Player::UpdateFlashlight"), xorstr_("Player::UpdateFlashlight"), reinterpret_cast<uintptr_t>(updateFlashlightHook), reinterpret_cast<uintptr_t*>(&oUpdateFlashlight));

	VIRTUALIZER_FISH_RED_END
}

bool Player::HasInitialized()
{
	return Memory::Objects[xorstr_("Player::Mode")] && Memory::Objects[xorstr_("Player::Flag")];
}

uintptr_t Player::GetInstance()
{
	const uintptr_t instanceAddress = Memory::Objects[xorstr_("Player::Instance")];

	return instanceAddress ? *reinterpret_cast<uintptr_t*>(instanceAddress) : 0u;
}

bool Player::GetIsLoaded()
{
	const uintptr_t instance = GetInstance();

	return instance && *reinterpret_cast<bool*>(instance + ASYNC_LOAD_COMPLETE_OFFSET);
}

bool Player::GetIsReplayMode()
{
	const uintptr_t instance = GetInstance();

	return instance ? *reinterpret_cast<bool*>(instance + REPLAY_MODE_OFFSET) : false;
}

PlayModes Player::GetPlayMode()
{
	auto modePtr = reinterpret_cast<PlayModes*>(Memory::Objects[xorstr_("Player::Mode")]);

	return modePtr ? *modePtr : PlayModes::Osu;
}

bool Player::GetIsRetrying()
{
	const uintptr_t retryingAddress = Memory::Objects[xorstr_("Player::Retrying")];

	return retryingAddress ? *reinterpret_cast<bool*>(retryingAddress) : false;
}

bool Player::GetIsFailed()
{
	const uintptr_t failedAddress = Memory::Objects[xorstr_("Player::Failed")];

	return failedAddress ? *reinterpret_cast<bool*>(failedAddress) : false;
}

int Player::GetAnticheatFlag()
{
	const uintptr_t flagAddress = Memory::Objects[xorstr_("Player::Flag")];

	return flagAddress ? *reinterpret_cast<int*>(flagAddress) : 0;
}

void Player::ResetAnticheatFlag()
{
	if (const uintptr_t flagAddress = Memory::Objects[xorstr_("Player::Flag")])
		*reinterpret_cast<int*>(flagAddress) = 0;
}
