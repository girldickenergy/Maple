#include "Score.h"

#include "Vanilla.h"

#include "../Memory.h"
#include "../../Config/Config.h"
#include "../Player/Player.h"
#include "../../UI/Windows/ScoreSubmissionDialog.h"
#include "../../Logging/Logger.h"
#include "../../Utilities/Security/xorstr.hpp"

void __fastcall Score::submitHook(uintptr_t instance)
{
	if (Player::GetAnticheatFlag() != 0)
		Logger::Log(LogSeverity::Warning, xor ("AC flag is not zero! Flag -> %d"), Player::GetAnticheatFlag());

	Player::ResetAnticheatFlag();

	if (Config::Misc::ScoreSubmissionType == 1 || Config::Misc::ForceDisableScoreSubmission)
		return;

	if (Config::Misc::ScoreSubmissionType == 2 && !Player::GetIsRetrying())
	{
		scoreInstance = instance;
		Vanilla::AddRelocation(std::ref(scoreInstance));

		ScoreSubmissionDialog::Show();
	}
	else oSubmit(instance);
}

void Score::Initialize()
{
	Memory::AddObject("Score::Submit", "55 8B EC 57 56 53 8B F1 83 BE ?? ?? ?? ?? 00 7E 05 5B 5E 5F");
	Memory::AddHook("Score::Submit", "Score::Submit", reinterpret_cast<uintptr_t>(submitHook), reinterpret_cast<uintptr_t*>(&oSubmit));
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
