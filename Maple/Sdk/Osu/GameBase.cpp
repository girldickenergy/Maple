#include "GameBase.h"

#include <Vanilla.h>

void GameBase::Initialize()
{
	RawGameBase = Vanilla::Explorer["osu.GameBase"];

	instanceAddress = RawGameBase["Instance"].Field.GetAddress();
	modeAddress = RawGameBase["Mode"].Field.GetAddress();
	clientBoundsField = RawGameBase["ClientBounds"].Field;

	RawGameBase["get_Window"].Method.Compile();
	getWindow = (fnGetWindow)RawGameBase["get_Window"].Method.GetNativeStart();

	Vanilla::Explorer["osu.Framework.WindowsGameWindow"]["get_Handle"].Method.Compile();
	getHandle = (fnGetHandle)Vanilla::Explorer["osu.Framework.WindowsGameWindow"]["get_Handle"].Method.GetNativeStart();
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

HWND GameBase::GetWindowHandle()
{
	void* windowInstance = getWindow(Instance());

	return getHandle(windowInstance);
}
