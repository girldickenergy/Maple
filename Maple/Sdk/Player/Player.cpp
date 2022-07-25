#include "Player.h"

#include "../Memory.h"
#include "../../Features/Timewarp/Timewarp.h"

bool __fastcall Player::onLoadCompleteHook(void* instance, bool success)
{
	if (success)
	{
		Timewarp::Initialize();
	}

	return oOnLoadComplete(instance, success);
}

void Player::Initialize()
{
	Memory::AddObject("Player::Instance", "FF 50 0C 8B D8 8B 15", 0x7, 1);
	Memory::AddObject("Player::Retrying", "8B CE FF 15 ?? ?? ?? ?? C6 05 ?? ?? ?? ?? 00", 0xA, 1);
	Memory::AddObject("Player::Flag", "E8 ?? ?? ?? ?? 33 D2 89 15 ?? ?? ?? ?? 88 15 ?? ?? ?? ?? B9", 0x9, 1);

	Memory::AddObject("Player::GetAllowSubmissionVariableConditions", "55 8B EC 56 8B F1 A1 ?? ?? ?? ?? 2B 86");
	Memory::AddPatch("Player::GetAllowSubmissionVariableConditions_HackCheck", "Player::GetAllowSubmissionVariableConditions", "83 BE ?? ?? ?? ?? 00 7E 1C", 0x80, 0x0, {0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90});

	Memory::AddObject("Player::HandleScoreSubmission", "55 8B EC 57 56 53 83 EC 08 8B F1 80 BE");
	Memory::AddPatch("Player::HandleScoreSubmission_HackCheck", "Player::HandleScoreSubmission", "80 78 7C 00 0F 84", 0x40F, 0x5, { 0x8D });

	Memory::AddObject("Player::Update", "55 8B EC 57 56 53 81 EC ?? ?? ?? ?? 8B F1 8D BD ?? ?? ?? ?? B9 ?? ?? ?? ?? 33 C0 F3 AB 8B CE 89 8D ?? ?? ?? ?? 8B 8D ?? ?? ?? ?? FF 15 ?? ?? ?? ?? 8B 8D ?? ?? ?? ?? FF 15 ?? ?? ?? ?? 85 C0 74 05");
	Memory::AddPatch("Player::Update_AudioCheck", "Player::Update", "0F 85 ?? ?? ?? ?? 83 BE ?? ?? ?? ?? FF", 0x1BCC, 0x1, { 0x8D });

	Memory::AddObject("Player::CheckFlashlightHax", "55 8B EC 57 56 53 83 EC 18 8B F9 80 3D");
	Memory::AddPatch("Player::CheckFlashlightHax_FLCheck", "Player::CheckFlashlightHax", "80 3D ?? ?? ?? ?? 00 75 24 83 BF", 0x375, 0x7, { 0x7D });

	Memory::AddObject("Player::HaxCheckMouse", "55 8B EC 57 56 83 EC 54 80 3D");
	Memory::AddPatch("Player::HaxCheckMouse_MouseCheck", "Player::HaxCheckMouse", "80 3D ?? ?? ?? ?? 00 75 14", 0x2C0, 0x7, { 0x7D });

	Memory::AddObject("Player::CheckAimAssist", "55 8B EC 57 56 83 EC 24 8B F1 8D 86");
	Memory::AddPatch("Player::CheckAimAssist_AACheck", "Player::CheckAimAssist", "80 38 00 74 0B", 0x212, 0x3, { 0x7D });

	Memory::AddObject("Player::HaxCheckPass", "55 8B EC 57 56 53 83 EC 2C 8B F1 8D 7D C8");
	Memory::AddPatch("Player::HaxCheckPass_UniqueIDCheck", "Player::HaxCheckPass", "EB 02 33 C0 85 C0 75 13", 0x400, 0x8, { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 });

	Memory::AddObject("Player::OnLoadComplete", "55 8B EC 57 56 53 83 EC 54 8B F1 8D 7D AC");
	Memory::AddHook("Player::OnLoadComplete", "Player::OnLoadComplete", reinterpret_cast<uintptr_t>(onLoadCompleteHook), reinterpret_cast<uintptr_t*>(&oOnLoadComplete));
}

uintptr_t Player::GetInstance()
{
	const uintptr_t instanceAddress = Memory::Objects["Player::Instance"];

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
	const uintptr_t instance = GetInstance();

	return instance ? *reinterpret_cast<PlayModes*>(instance + PLAY_MODE_OFFSET) : PlayModes::Osu;
}

bool Player::GetIsRetrying()
{
	const uintptr_t retryingAddress = Memory::Objects["Player::Retrying"];

	return retryingAddress ? *reinterpret_cast<bool*>(retryingAddress) : false;
}

int Player::GetAnticheatFlag()
{
	const uintptr_t flagAddress = Memory::Objects["Player::Flag"];

	return flagAddress ? *reinterpret_cast<int*>(flagAddress) : 0;
}

void Player::ResetAnticheatFlag()
{
	if (const uintptr_t flagAddress = Memory::Objects["Player::Flag"])
		*reinterpret_cast<int*>(flagAddress) = 0;
}
