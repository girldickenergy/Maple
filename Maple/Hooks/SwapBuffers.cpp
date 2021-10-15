#include "Hooks.h"

#include "../UI/Overlay.h"

BOOL __stdcall Hooks::wglSwapBuffersHook(HDC hdc)
{
	if (!Overlay::Initialized)
		Overlay::Initialize();

	Overlay::Render();

	return oWglSwapBuffers(hdc);
}