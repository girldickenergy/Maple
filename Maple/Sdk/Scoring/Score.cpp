#include "Score.h"

#include "Vanilla.h"
#include "xorstr.hpp"

#include "../../Configuration/ConfigManager.h"
#include "../Player/Player.h"
#include "../../UI/Windows/ScoreSubmissionDialog.h"
#include "../../Logging/Logger.h"

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
	oSubmit(scoreInstance);

	Vanilla::RemoveRelocation(std::ref(scoreInstance));
}

void Score::AbortSubmission()
{
	Vanilla::RemoveRelocation(std::ref(scoreInstance));
}
