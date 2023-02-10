#include "Score.h"
#include "ThemidaSDK.h"
#include "Vanilla.h"

#include "../Memory.h"
#include "../../Config/Config.h"
#include "../Player/Player.h"
#include "../../UI/Windows/ScoreSubmissionDialog.h"
#include "../../Logging/Logger.h"
#include "../../Utilities/Security/xorstr.hpp"
#include "../Osu/GameBase.h"
#include "../../Features/Timewarp/Timewarp.h"
#include "../../Communication/Communication.h"

void Score::spoofPlayDuration()
{
	const int playStartTime = *reinterpret_cast<int*>(scoreInstance + STARTTIME_OFFSET);
	const int playEndTime = GameBase::GetTime();
	const int playDuration = playEndTime - playStartTime;
	const int scaledDuration = static_cast<int>(playDuration * Timewarp::GetRateMultiplier());

	*reinterpret_cast<int*>(scoreInstance + STARTTIME_OFFSET) = playEndTime - scaledDuration;
}

#pragma optimize("", off)
int Score::handleScoreSubmission()
{
	if (Player::GetAnticheatFlag() != 0)
		Logger::Log(LogSeverity::Warning, xorstr_("AC flag is not zero! Flag -> %d"), Player::GetAnticheatFlag());

	Player::ResetAnticheatFlag();

	if (Config::Misc::ScoreSubmissionType == 1 || Config::Misc::ForceDisableScoreSubmission)
		return 0;

	if (Config::Misc::ScoreSubmissionType == 2 && !Player::GetIsRetrying())
	{
		Vanilla::AddRelocation(std::ref(scoreInstance));

		ScoreSubmissionDialog::Show();

		return 0;
	}

	if (Config::Misc::ScoreSubmissionType == 2 && Player::GetIsRetrying() && Config::Misc::PromptBehaviorOnRetry == 1)
		return 0;

	if (Config::Timewarp::Enabled)
		spoofPlayDuration();

	return 1;
}
#pragma optimize("", on)

void __declspec(naked) Score::submitHook(uintptr_t instance)
{
	__asm
	{
		mov [scoreInstance], ecx

		pushad
		pushfd
		call handleScoreSubmission
		cmp eax, 0
		je skipSubmission
		popfd
		popad
		jmp oSubmit
		skipSubmission:
		popfd
		popad
		ret
	}
}

void Score::Initialize()
{
	VM_FISH_RED_START
	STR_ENCRYPT_START

	Memory::AddObject(xorstr_("Score::Submit"), xorstr_("E9 ?? ?? ?? ?? 53 8B F1 83 BE ?? ?? ?? ?? 00 7E 05 5B 5E 5F"));
	Memory::AddHook(xorstr_("Score::Submit"), xorstr_("Score::Submit"), reinterpret_cast<uintptr_t>(submitHook), reinterpret_cast<uintptr_t*>(&oSubmit), true);

	STR_ENCRYPT_END
	VM_FISH_RED_END
}

void Score::Submit()
{
	if (Config::Timewarp::Enabled)
		spoofPlayDuration();

	oSubmit(scoreInstance);

	Vanilla::RemoveRelocation(std::ref(scoreInstance));
}

void Score::AbortSubmission()
{
	Vanilla::RemoveRelocation(std::ref(scoreInstance));
}
