#include "Score.h"
#include "VirtualizerSDK.h"
#include "Vanilla.h"
#include "xorstr.hpp"

#include "../Memory.h"
#include "../../Config/Config.h"
#include "../Player/Player.h"
#include "../../UI/Windows/ScoreSubmissionDialog.h"
#include "../../Logging/Logger.h"
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

void __fastcall Score::submitHook(uintptr_t instance)
{
	scoreInstance = instance;

	Logger::Log(LogSeverity::Debug, xorstr_("A"));

	if (Player::GetAnticheatFlag() != 0)
		Logger::Log(LogSeverity::Warning, xorstr_("AC flag is not zero! Flag -> %d"), Player::GetAnticheatFlag());

	Logger::Log(LogSeverity::Debug, xorstr_("A.A"));

	Player::ResetAnticheatFlag();

	Logger::Log(LogSeverity::Debug, xorstr_("A.B"));

	if (Config::Misc::ScoreSubmissionType == 1 || Config::Misc::ForceDisableScoreSubmission)
		return;

	Logger::Log(LogSeverity::Debug, xorstr_("B"));

	if (Config::Misc::ScoreSubmissionType == 2 && !Player::GetIsRetrying())
	{
		Vanilla::AddRelocation(std::ref(scoreInstance));

		ScoreSubmissionDialog::Show();

		Logger::Log(LogSeverity::Debug, xorstr_("B.A"));

		return;
	}

	Logger::Log(LogSeverity::Debug, xorstr_("C"));

	if (Config::Misc::ScoreSubmissionType == 2 && Player::GetIsRetrying() && Config::Misc::PromptBehaviorOnRetry == 1)
		return;

	Logger::Log(LogSeverity::Debug, xorstr_("D"));

	if (Config::Timewarp::Enabled)
		spoofPlayDuration();

	Logger::Log(LogSeverity::Debug, xorstr_("E"));
	[[clang::musttail]] return oSubmit(instance);
}

void Score::Initialize()
{
	VIRTUALIZER_FISH_RED_START
	
	Memory::AddObject(xorstr_("Score::Submit"), xorstr_("E9 ?? ?? ?? ?? 53 8B F1 83 BE ?? ?? ?? ?? 00 7E 05 5B 5E 5F"), 1, 1, true);
	Memory::AddHook(xorstr_("Score::Submit"), xorstr_("Score::Submit"), reinterpret_cast<uintptr_t>(submitHook), reinterpret_cast<uintptr_t*>(&oSubmit), true);

	VIRTUALIZER_FISH_RED_END
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
