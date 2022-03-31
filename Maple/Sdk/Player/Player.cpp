#include "Player.h"

#include <Vanilla.h>

#include "../../Features/AimAssist/AimAssist.h"
#include "../../Features/Relax/Relax.h"
#include "../../Features/Timewarp/Timewarp.h"
#include "../../UI/Menus/MainMenu.h"
#include "../../Features/Visuals/VisualsSpoofers.h"
#include "../../Dependencies/Chiyo/Decoders/ReplayDecoder.h"
#include "../../Features/ReplayBot/ReplayBot.h"

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

	return instance && (*static_cast<bool*>(asyncLoadCompleteField.GetAddress(instance)) || isLoaded);
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

void __fastcall Player::DisposeHook(void* instance, BOOL disposing)
{
	isLoaded = false;

	oDispose(instance, disposing);
}

BOOL __fastcall Player::OnPlayerLoadCompleteHook(void* instance, BOOL success)
{
	if (success)
	{
		MainMenu::IsOpen = false;

		HitObjectManager::CacheAllHitObjects();

		ReplayBot::Initialize();

		if (!ReplayBot::Ready || ReplayBot::DisableTapping)
			Relax::Initialize();

		if (!ReplayBot::Ready || ReplayBot::DisableAiming)
			AimAssist::Initialize();

		Timewarp::UpdateCatcherSpeed();
		VisualsSpoofers::LoadPreemptiveDots();

		isLoaded = true;
	}
	
	return oOnPlayerLoadComplete(instance, success);
}
