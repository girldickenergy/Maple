#pragma once

#include <d3d9.h>
#include <d3dx9.h>

class Rendering
{
	typedef int(__stdcall* fnWglSwapBuffers)(_In_ HDC hdc);
	typedef int(__stdcall* fnPresent)(IDirect3DSwapChain9* pSwapChain, const RECT* pSourceRect, const RECT* pDestRect, HWND hDestWindowOverride, const RGNDATA* pDirtyRegion, DWORD dwFlags);
public:
	static inline fnWglSwapBuffers oWglSwapBuffers;
	static BOOL WINAPI WglSwapBuffersHook(_In_ HDC hdc);

	static inline fnPresent oPresent;
	static int __stdcall PresentHook(IDirect3DSwapChain9* pSwapChain, const RECT* pSourceRect, const RECT* pDestRect, HWND hDestWindowOverride, const RGNDATA* pDirtyRegion, DWORD dwFlags);
};