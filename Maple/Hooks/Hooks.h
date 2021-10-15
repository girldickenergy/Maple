#pragma once

#include <Vanilla.h>
#include <Hook.h>
#include <CinnamonResult.h>
#include <d3d9.h>

typedef BOOL(WINAPI* fnWglSwapBuffers) (_In_ HDC hdc);
typedef HRESULT(WINAPI* fnEndScene) (IDirect3DDevice9* pDevice);

class Hooks
{
	static inline fnWglSwapBuffers oWglSwapBuffers;
	static BOOL WINAPI wglSwapBuffersHook(_In_ HDC hdc);
	
	static inline fnEndScene oEndScene;
	static HRESULT WINAPI endSceneHook(IDirect3DDevice9* pDevice);
	
	static CinnamonResult installManagedHook(std::string name, Method method, LPVOID pDetour, LPVOID* ppOriginal, HookType hookType = HookType::ByteCodePatch);
public:
	static void InstallAllHooks();
	static void UninstallAllHooks();
};
