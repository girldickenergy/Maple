#include "Timewarp.h"

#include "../../Sdk/Audio/AudioEngine.h"
#include "../../Sdk/Player/Player.h"

void Timewarp::Initialize()
{
}

void Timewarp::TimewarpThread()
{
	while (true)
	{
        if (Player::IsLoaded() && !Player::IsReplayMode())
        {
            if (round(AudioEngine::GetPlaybackRate()) != 85.)
            {
                AudioEngine::SetPlaybackRate(85);
            }
        }

        Sleep(100);
	}
}
