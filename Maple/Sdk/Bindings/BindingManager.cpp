#include "BindingManager.h"

#include <Vanilla.h>

void BindingManager::Initialize()
{
	RawBindingManager = Vanilla::Explorer["osu.Input.BindingManager"];

	RawBindingManager["GetPlayKey"].Method.Compile();
	getPlayKey = static_cast<fnGetPlayKey>(RawBindingManager["GetPlayKey"].Method.GetNativeStart());
}

int BindingManager::GetPlayKey(PlayKeys key)
{
	return getPlayKey(key);
}
