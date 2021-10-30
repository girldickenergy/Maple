#include "ScoreSubmission.h"

#include "../../Config/Config.h"
#include "../../Sdk/Osu/GameBase.h"
#include "../../Sdk/Player/Player.h"
#include "../../UI/Overlay.h"

void __fastcall ScoreSubmission::SubmitHook(void* instance)
{
	scoreInstance = instance;
	
	if (Config::Misc::PromptOnScoreSubmissionEnabled /*&& GameBase::HasLogin()*/ && !Player::IsRetrying())
		Overlay::ShowScoreSubmissionDialogue();
	else
		oSubmit(instance);
}

void ScoreSubmission::Submit()
{
	oSubmit(scoreInstance);
}
