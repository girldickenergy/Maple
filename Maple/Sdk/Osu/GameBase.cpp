#include "GameBase.h"

#include <Vanilla.h>

void GameBase::Initialize()
{
	RawGameBase = Vanilla::Explorer["osu.GameBase"];

	RawGameBase["get_HasLogin"].Method.Compile();
	hasLogin = static_cast<fnHasLogin>(RawGameBase["get_HasLogin"].Method.GetNativeStart());

	instanceAddress = RawGameBase["Instance"].Field.GetAddress();
	modeAddress = RawGameBase["Mode"].Field.GetAddress();
}

void* GameBase::Instance()
{
	return *static_cast<void**>(instanceAddress);
}

OsuModes GameBase::Mode()
{
	return *static_cast<OsuModes*>(modeAddress);
}

bool GameBase::HasLogin()
{
	return hasLogin();
}
