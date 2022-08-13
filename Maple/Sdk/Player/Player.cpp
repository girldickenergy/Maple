#include "Player.h"

#include "ThemidaSDK.h"

#include "HitObjectManager.h"
#include "../Memory.h"
#include "../../Features/Timewarp/Timewarp.h"
#include "../../Features/ReplayBot/ReplayBot.h"
#include "../../Features/Enlighten/Enlighten.h"
#include "../../UI/Windows/MainMenu.h"
#include "../../Features/AimAssist/AimAssist.h"
#include "../../Features/Relax/Relax.h"
#include "../../Config/Config.h"
#include "../Osu/GameBase.h"
#include "../../Utilities/Security/xorstr.hpp"

void Player::initializeFeatures()
{
	MainMenu::Hide();
	
	HitObjectManager::CacheHitObjects();

	Enlighten::Initialize();
	Timewarp::Initialize();
	Relax::Initialize();
	AimAssist::Initialize();
	ReplayBot::Initialize();
}

int __declspec(naked) Player::onLoadCompleteHook(uintptr_t instance, bool success)
{
	__asm
	{
		pushad
		pushfd
		cmp edx, 0
		je orig
		call initializeFeatures
		orig:
		popfd
		popad
		jmp oOnLoadComplete
	}
}

void __declspec(naked) Player::updateFlashlightHook(uintptr_t instance)
{
	__asm
	{
		pushad
		pushfd
		cmp [Config::Visuals::Removers::FlashlightRemoverEnabled], 0x0
		je orig
		call GameBase::GetMode
		cmp eax, 0x2
		jne orig
		call GetIsReplayMode
		cmp al, 0x0
		jne orig
		popfd
		popad
		ret
		orig:
		popfd
		popad
		jmp oUpdateFlashlight
	}
}

void Player::Initialize()
{
	Memory::AddObject(xor ("Player::Instance"), xor ("FF 50 0C 8B D8 8B 15"), 0x7, 1);
	Memory::AddObject(xor ("Player::Retrying"), xor ("8B CE FF 15 ?? ?? ?? ?? C6 05 ?? ?? ?? ?? 00"), 0xA, 1);
	Memory::AddObject(xor ("Player::Failed"), xor ("8B 15 ?? ?? ?? ?? 89 90 ?? ?? ?? ?? 80 3D ?? ?? ?? ?? 00 74 57 80 3D"), 0xE, 1);
	Memory::AddObject(xor ("Player::Flag"), xor ("E8 ?? ?? ?? ?? 33 D2 89 15 ?? ?? ?? ?? 88 15 ?? ?? ?? ?? B9"), 0x9, 1);

	Memory::AddObject(xor ("Player::GetAllowSubmissionVariableConditions"), xor ("55 8B EC 56 8B F1 A1 ?? ?? ?? ?? 2B 86"));
	Memory::AddPatch(xor ("Player::GetAllowSubmissionVariableConditions_HackCheck"), xor ("Player::GetAllowSubmissionVariableConditions"), xor ("83 BE ?? ?? ?? ?? 00 7E 1C"), 0x80, 0x0, {0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90});

	Memory::AddObject(xor ("Player::HandleScoreSubmission"), xor ("55 8B EC 57 56 53 83 EC 08 8B F1 80 BE ?? ?? ?? ?? 00 75 26 B9"));
	Memory::AddPatch(xor ("Player::HandleScoreSubmission_HackCheck"), xor ("Player::HandleScoreSubmission"), xor ("80 78 7C 00 0F 84"), 0x40F, 0x5, { 0x8D });

	Memory::AddObject(xor ("Player::Update"), xor ("55 8B EC 57 56 53 81 EC ?? ?? ?? ?? 8B F1 8D BD ?? ?? ?? ?? B9 ?? ?? ?? ?? 33 C0 F3 AB 8B CE 89 8D ?? ?? ?? ?? 8B 8D ?? ?? ?? ?? FF 15 ?? ?? ?? ?? 8B 8D ?? ?? ?? ?? FF 15 ?? ?? ?? ?? 85 C0 74 05"));
	Memory::AddPatch(xor ("Player::Update_AudioCheck"), xor ("Player::Update"), xor ("0F 85 ?? ?? ?? ?? 83 BE ?? ?? ?? ?? FF"), 0x1BCC, 0x1, { 0x8D });

	Memory::AddObject(xor ("Player::CheckFlashlightHax"), xor ("55 8B EC 57 56 53 83 EC 18 8B F9 80 3D"));
	Memory::AddPatch(xor ("Player::CheckFlashlightHax_FLCheck"), xor ("Player::CheckFlashlightHax"), xor ("80 3D ?? ?? ?? ?? 00 75 24 83 BF"), 0x375, 0x7, { 0x7D });

	Memory::AddObject(xor ("Player::HaxCheckMouse"), xor ("55 8B EC 57 56 83 EC 54 80 3D"));
	Memory::AddPatch(xor ("Player::HaxCheckMouse_MouseCheck"), xor ("Player::HaxCheckMouse"), xor ("80 3D ?? ?? ?? ?? 00 75 14"), 0x2C0, 0x7, { 0x7D });

	Memory::AddObject(xor ("Player::CheckAimAssist"), xor ("55 8B EC 57 56 83 EC 24 8B F1 8D 86"));
	Memory::AddPatch(xor ("Player::CheckAimAssist_AACheck"), xor ("Player::CheckAimAssist"), xor ("80 38 00 74 0B"), 0x212, 0x3, { 0x7D });

	Memory::AddObject(xor ("Player::HaxCheckPass"), xor ("55 8B EC 57 56 53 83 EC 2C 8B F1 8D 7D C8 B9 ?? ?? ?? ?? 33 C0 F3 AB 8B CE 83 3D ?? ?? ?? ?? 00 75 0A B8"));
	Memory::AddPatch(xor ("Player::HaxCheckPass_UniqueIDCheck"), xor ("Player::HaxCheckPass"), xor ("EB 02 33 C0 85 C0 75 13"), 0x400, 0x8, { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 });

	Memory::AddObject(xor ("Player::OnLoadComplete"), xor ("55 8B EC 57 56 53 83 EC 54 8B F1 8D 7D AC B9 ?? ?? ?? ?? 33 C0 F3 AB 8B CE 89 4D B0 33 C9 89 0D"));
	Memory::AddHook(xor ("Player::OnLoadComplete"), xor ("Player::OnLoadComplete"), reinterpret_cast<uintptr_t>(onLoadCompleteHook), reinterpret_cast<uintptr_t*>(&oOnLoadComplete));

	Memory::AddObject(xor ("Player::UpdateFlashlight"), xor ("55 8B EC 57 56 8B F1 83 BE ?? ?? ?? ?? 00 74 32 83 7E 60 00 74 2C A1 ?? ?? ?? ?? 8B 50 1C"));
	Memory::AddHook(xor ("Player::UpdateFlashlight"), xor ("Player::UpdateFlashlight"), reinterpret_cast<uintptr_t>(updateFlashlightHook), reinterpret_cast<uintptr_t*>(&oUpdateFlashlight));
}

uintptr_t Player::GetInstance()
{
	const uintptr_t instanceAddress = Memory::Objects[xor ("Player::Instance")];

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
	const uintptr_t retryingAddress = Memory::Objects[xor ("Player::Retrying")];

	return retryingAddress ? *reinterpret_cast<bool*>(retryingAddress) : false;
}

bool Player::GetIsFailed()
{
	const uintptr_t failedAddress = Memory::Objects[xor ("Player::Failed")];

	return failedAddress ? *reinterpret_cast<bool*>(failedAddress) : false;
}

int Player::GetAnticheatFlag()
{
	const uintptr_t flagAddress = Memory::Objects[xor ("Player::Flag")];

	return flagAddress ? *reinterpret_cast<int*>(flagAddress) : 0;
}

void Player::ResetAnticheatFlag()
{
	if (const uintptr_t flagAddress = Memory::Objects[xor ("Player::Flag")])
		*reinterpret_cast<int*>(flagAddress) = 0;
}
