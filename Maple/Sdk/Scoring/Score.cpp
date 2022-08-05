#include "Score.h"

#include "Vanilla.h"

#include "../Memory.h"
#include "../../Config/Config.h"
#include "../Player/Player.h"
#include "../../UI/Windows/ScoreSubmissionDialog.h"
#include "../../Logging/Logger.h"
#include "../../Utilities/Security/xorstr.hpp"

void Score::checkFlags()
{
	if (Player::GetAnticheatFlag() != 0)
		Logger::Log(LogSeverity::Warning, xor ("AC flag is not zero! Flag -> %d"), Player::GetAnticheatFlag());

	Player::ResetAnticheatFlag();
}

void Score::handleScoreSubmissionPrompt()
{
	Vanilla::AddRelocation(std::ref(scoreInstance));

	ScoreSubmissionDialog::Show();
}

void __declspec(naked) Score::submitHook(uintptr_t instance)
{
	__asm
	{
		mov [scoreInstance], ecx

		pushad
		pushfd
		call checkFlags
		popfd
		popad

		pushad
		pushfd
		cmp [Config::Misc::ScoreSubmissionType], 0x1
		je end
		cmp [Config::Misc::ForceDisableScoreSubmission], 0x1
		je end
		cmp [Config::Misc::ScoreSubmissionType], 0x2
		jne orig
		call Player::GetIsRetrying
		cmp eax, 0x0
		jne orig
		call handleScoreSubmissionPrompt
		end:
		popfd
		popad
		ret
		orig:
		popfd
		popad
		jmp oSubmit
	}
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
