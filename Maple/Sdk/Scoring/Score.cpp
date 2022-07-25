#include "Score.h"

#include "../Memory.h"
#include "../../Config/Config.h"
#include "../Player/Player.h"
#include "../../Logging/Logger.h"
#include "../../Utilities/Security/xorstr.hpp"

void __fastcall Score::submitHook(uintptr_t instance)
{
	if (Player::GetAnticheatFlag() != 0)
		Logger::Log(LogSeverity::Warning, xor ("AC flag is not zero! Flag -> %d"), Player::GetAnticheatFlag());

	Player::ResetAnticheatFlag();

	if (Config::Misc::DisableScoreSubmission || Config::Misc::ForceDisableSubmission)
		return;

	oSubmit(instance);
}

void Score::Initialize()
{
	Memory::AddObject("Score::Submit", "55 8B EC 57 56 53 8B F1 83 BE ?? ?? ?? ?? 00 7E 05 5B 5E 5F");
	Memory::AddHook("Score::Submit", "Score::Submit", reinterpret_cast<uintptr_t>(submitHook), reinterpret_cast<uintptr_t*>(&oSubmit));
}
