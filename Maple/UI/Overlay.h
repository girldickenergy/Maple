#pragma once

#include <d3d9.h>

enum class Renderer
{
	OGL3,
	D3D9
};

typedef BOOL(WINAPI* fnGetKeyboardState) (PBYTE arr);

class Overlay
{
	static inline HWND window;
	
	static inline bool rawInputDisabled = false;

	static inline RAWINPUTDEVICE* pDevices;
	static inline UINT uiDeviceCount;

	static void enableRaw();
	static void disableRaw();
	static void backupRaw();
public:
	static inline Renderer Renderer = Renderer::OGL3;
	static inline IDirect3DDevice9* D3D9Device = nullptr;
	static inline bool Initialized = false;
	
	static void Initialize(IDirect3DDevice9* d3d9Device = nullptr);
	static void Render();
	static void ToggleMainMenu();
	static void ShowScoreSubmissionDialogue();
	static void HideScoreSubmissionDialogue();

	static inline HHOOK oHandleInput;
	static LRESULT CALLBACK HandleInputHook(int nCode, WPARAM wParam, LPARAM lParam);
	
	static inline fnGetKeyboardState oHandleKeyboardInput;
	static BOOL WINAPI HandleKeyboardInputHook(PBYTE arr);
};