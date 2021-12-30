#include "Player.h"

#include <Vanilla.h>

#include "../../Features/AimAssist/AimAssist.h"
#include "../../Features/Relax/Relax.h"
#include "../../Features/Timewarp/Timewarp.h"
#include "../../UI/MainMenu.h"

void Player::Initialize()
{
	RawPlayer = Vanilla::Explorer["osu.GameModes.Play.Player"];
	asyncLoadCompleteField = RawPlayer["AsyncLoadComplete"].Field;
	replayModeStableField = RawPlayer["replayModeStable"].Field;

	instanceAddress = RawPlayer["Instance"].Field.GetAddress();
	isRetryingAddress = RawPlayer["Retrying"].Field.GetAddress();
	modeAddress = RawPlayer["mode"].Field.GetAddress();
	playingAddress = RawPlayer["Playing"].Field.GetAddress();
	pausedAddress = RawPlayer["Paused"].Field.GetAddress();
}

void* Player::Instance()
{
	return *static_cast<void**>(instanceAddress);
}

bool Player::IsLoaded()
{
	void* instance = Instance();
	if (!instance)
		loadComplete = false;
	
	return instance && (*static_cast<bool*>(asyncLoadCompleteField.GetAddress(instance)) || loadComplete);
}

bool Player::IsReplayMode()
{
	void* instance = Instance();
	
	if (!instance)
		return false;
	
	return *static_cast<bool*>(replayModeStableField.GetAddress(instance));
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

bool Player::IsPaused()
{
	return *static_cast<bool*>(pausedAddress);
}

void __fastcall Player::PlayerInitialize(uintptr_t instance)
{
	MainMenu::IsOpen = false;
	
	Relax::Stop();

	oPlayerInitialize(instance);
}

BOOL __fastcall Player::OnPlayerLoadCompleteHook(void* instance, BOOL success)
{
	if (success)
	{
		loadComplete = true;

		Relax::Start();
		AimAssist::Reset();
		Timewarp::UpdateCatcherSpeed();
	}
	
	return oOnPlayerLoadComplete(instance, success);
}
