#include "VisualsSpoofers.h"

#include "../../Config/Config.h"
#include "../../Sdk/Player/Player.h"
#include "../../Sdk/Player/Ruleset.h"

void VisualsSpoofers::FlashlightRemoverThread()
{
	while (true)
	{
		if (Player::IsLoaded() && !Player::IsReplayMode())
		{
			const float targetAlpha = roundf(Config::Visuals::FlashlightDisabled ? 0.f : 1.f);
			if (roundf(Ruleset::GetFlashlightAlpha()) != targetAlpha)
				Ruleset::SetFlashlightAlpha(targetAlpha);
		}

		Sleep(100);
	}
}
