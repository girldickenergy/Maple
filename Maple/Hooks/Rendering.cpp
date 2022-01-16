#include "Rendering.h"

#include "../UI/Overlay.h"

BOOL __stdcall Rendering::WglSwapBuffersHook(HDC hdc)
{
	if (!Overlay::Initialized)
		Overlay::Initialize();

	Overlay::Render();

	return oWglSwapBuffers(hdc);
}

int __stdcall Rendering::PresentHook(IDirect3DSwapChain9* pSwapChain, const RECT* pSourceRect, const RECT* pDestRect, HWND hDestWindowOverride, const RGNDATA* pDirtyRegion, DWORD dwFlags)
{
	if (!Overlay::Initialized)
	{
		IDirect3DDevice9* d3dDevice;
		pSwapChain->GetDevice(&d3dDevice);

		Overlay::Initialize(d3dDevice);
	}

	Overlay::Render();

	return oPresent(pSwapChain, pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion, dwFlags);
}
