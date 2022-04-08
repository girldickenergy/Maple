#include "ScoreSubmission.h"

#include <COM/Helpers/GCHelpers.h>

#include "../../Config/Config.h"
#include "../../Sdk/Anticheat/Anticheat.h"
#include "../../Sdk/Player/Player.h"
#include "../../UI/Overlay.h"
#include "../../Logging/Logger.h"
#include "../../Utilities/Security/xorstr.hpp"

void __fastcall ScoreSubmission::SubmitHook(void* instance)
{
	if (Anticheat::GetFlag() != 0)
		Logger::Log(LogSeverity::Warning, xor("AC flag is not zero! Flag -> %d"), Anticheat::GetFlag());

	Anticheat::ResetFlag();
	
	if (Config::Misc::PromptOnScoreSubmissionEnabled && !Player::IsRetrying())
	{
		if (GCHelpers::SetGCLatencyMode(2) == 0) //LowLatency
			canRestoreGC = true;
		else
			canRestoreGC = false;

		scoreInstance = instance;
		
		Overlay::ShowScoreSubmissionDialogue();
	}
	else
		oSubmit(instance);
}

void ScoreSubmission::Submit()
{
	oSubmit(scoreInstance);

	if (canRestoreGC)
		GCHelpers::SetGCLatencyMode(1); //Interactive
}

void ScoreSubmission::AbortSubmission()
{
	if (canRestoreGC)
		GCHelpers::SetGCLatencyMode(1); //Interactive
}
