#include "Player.h"

#include <Vanilla.h>

void Player::Initialize()
{
	RawPlayer = Vanilla::Explorer["osu.GameModes.Play.Player"];
	asyncLoadComplete = RawPlayer["AsyncLoadComplete"].Field;
	replayModeStable = RawPlayer["replayModeStable"].Field;

	instanceAddress = RawPlayer["Instance"].Field.GetAddress();
	isRetryingAddress = RawPlayer["Retrying"].Field.GetAddress();
	modeAddress = RawPlayer["mode"].Field.GetAddress();
	playingAddress = RawPlayer["Playing"].Field.GetAddress();
}

void* Player::Instance()
{
	return *static_cast<void**>(instanceAddress);
}

bool Player::IsLoaded()
{
	void* instance = Instance();
	
	return instance && *static_cast<bool*>(asyncLoadComplete.GetAddress(instance));
}

bool Player::IsReplayMode()
{
	return *static_cast<bool*>(replayModeStable.GetAddress(Instance()));
}

bool Player::IsRetrying()
{
	return *static_cast<bool*>(isRetryingAddress);
}

bool Player::IsPlaying()
{
	return *static_cast<bool*>(playingAddress);
}

PlayModes Player::PlayMode()
{
	return *static_cast<PlayModes*>(modeAddress);
}
