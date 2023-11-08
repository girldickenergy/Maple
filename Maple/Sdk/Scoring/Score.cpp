#include "Score.h"
#include "VirtualizerSDK.h"
#include "Vanilla.h"
#include "xorstr.hpp"

#include "../Memory.h"
#include "../../Configuration/ConfigManager.h"
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

	if (ConfigManager::CurrentConfig.Misc.ScoreSubmissionType == 1 || ConfigManager::ForceDisableScoreSubmission)
		return;

	Logger::Log(LogSeverity::Debug, xorstr_("B"));

	if (ConfigManager::CurrentConfig.Misc.ScoreSubmissionType == 2 && !Player::GetIsRetrying())
	{
		Vanilla::AddRelocation(std::ref(scoreInstance));

		ScoreSubmissionDialog::Show();

		Logger::Log(LogSeverity::Debug, xorstr_("B.A"));

		return;
	}

	Logger::Log(LogSeverity::Debug, xorstr_("C"));

	if (ConfigManager::CurrentConfig.Misc.ScoreSubmissionType == 2 && Player::GetIsRetrying() && ConfigManager::CurrentConfig.Misc.PromptBehaviorOnRetry == 1)
		return;

	Logger::Log(LogSeverity::Debug, xorstr_("D"));

	if (ConfigManager::CurrentConfig.Timewarp.Enabled)
		spoofPlayDuration();

	Logger::Log(LogSeverity::Debug, xorstr_("E"));
	[[clang::musttail]] return oSubmit(instance);
}

void Score::Initialize()
{

}

void* Score::GetHook()
{
	return submitHook;
}

void Score::SetOriginal(void* val)
{
	oSubmit = static_cast<fnSubmit>(val);
}

void Score::Submit()
{
	if (ConfigManager::CurrentConfig.Timewarp.Enabled)
		spoofPlayDuration();

	oSubmit(scoreInstance);

	Vanilla::RemoveRelocation(std::ref(scoreInstance));
}

void Score::AbortSubmission()
{
	Vanilla::RemoveRelocation(std::ref(scoreInstance));
}
