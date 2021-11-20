#include "GameBase.h"

#include <Vanilla.h>

void GameBase::Initialize()
{
	RawGameBase = Vanilla::Explorer["osu.GameBase"];

	instanceAddress = RawGameBase["Instance"].Field.GetAddress();
	modeAddress = RawGameBase["Mode"].Field.GetAddress();
	clientBoundsField = RawGameBase["ClientBounds"].Field;
}

void* GameBase::Instance()
{
	return *static_cast<void**>(instanceAddress);
}

OsuModes GameBase::Mode()
{
	return *static_cast<OsuModes*>(modeAddress);
}

sRectangle* GameBase::GetClientBounds()
{
	return static_cast<sRectangle*>(clientBoundsField.GetAddress());
}