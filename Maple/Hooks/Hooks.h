#pragma once

#include <Vanilla.h>
#include <Hook.h>
#include <CinnamonResult.h>
#include <d3d9.h>
#include <COM/COMString.h>

typedef BOOL(WINAPI* fnWglSwapBuffers) (_In_ HDC hdc);
typedef HRESULT(WINAPI* fnEndScene) (IDirect3DDevice9* pDevice);
typedef void(__fastcall* fnSubmitError)(void* err);
typedef void(__fastcall* fnAddParameter)(void* instance, COMString* name, COMString* value);

class Hooks
{
	static inline fnWglSwapBuffers oWglSwapBuffers;
	static BOOL WINAPI wglSwapBuffersHook(_In_ HDC hdc);
	
	static inline fnEndScene oEndScene;
	static HRESULT WINAPI endSceneHook(IDirect3DDevice9* pDevice);

	static inline fnSubmitError oSubmitError;
	static void __fastcall SubmitErrorHook(void* err);

	static inline fnAddParameter oAddParameter;
	static void __fastcall AddParameterHook(void* instance, COMString* name, COMString* value);
	
	static CinnamonResult installManagedHook(std::string name, Method method, LPVOID pDetour, LPVOID* ppOriginal, HookType hookType = HookType::ByteCodePatch);
public:
	static void InstallAllHooks();
	static void UninstallAllHooks();
};
