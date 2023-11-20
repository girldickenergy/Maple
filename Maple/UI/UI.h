#pragma once

#include <d3d9.h>

#include "Renderer.h"

class UI
{
	static inline HHOOK oWndProc;
	static LRESULT CALLBACK wndProcHook(int nCode, WPARAM wParam, LPARAM lParam);

	static inline bool bypassGameInput = false;

	typedef BOOL(WINAPI* fnGetKeyboardState) (PBYTE arr);
	static inline fnGetKeyboardState oGetKeyboardState;
	static BOOL WINAPI getKeyboardStateHook(PBYTE arr);

	typedef int(__stdcall* fnWglSwapBuffers)(_In_ HDC hdc);
	static inline fnWglSwapBuffers oWglSwapBuffers;
	static BOOL WINAPI wglSwapBuffersHook(_In_ HDC hdc);

	typedef int(__stdcall* fnPresent)(IDirect3DSwapChain9* pSwapChain, const RECT* pSourceRect, const RECT* pDestRect, HWND hDestWindowOverride, const RGNDATA* pDirtyRegion, DWORD dwFlags);
	static inline fnPresent oPresent;
	static int __stdcall presentHook(IDirect3DSwapChain9* pSwapChain, const RECT* pSourceRect, const RECT* pDestRect, HWND hDestWindowOverride, const RGNDATA* pDirtyRegion, DWORD dwFlags);

	static inline bool rawInputDisabled = false;
	static inline RAWINPUTDEVICE* pDevices;
	static inline UINT uiDeviceCount;

	static void enableRaw();
	static void disableRaw();
	static void backupRaw();

	static WPARAM mapLeftRightKeys(WPARAM vk, LPARAM lParam);

	static inline bool initialized = false;
	static void initialize(HWND window, IDirect3DDevice9* d3d9Device = nullptr);
	static void render();
public:
	static inline Renderer Renderer = Renderer::OGL3;
	static inline HWND Window = NULL;
	static inline IDirect3DDevice9* D3D9Device = nullptr;

	static void Initialize();
};
