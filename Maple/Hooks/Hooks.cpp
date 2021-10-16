#include "Hooks.h"

#include <Cinnamon.h>


#include "../Sdk/ConfigManager/ConfigManager.h"
#include "../UI/Overlay.h"
#include "../Utilities/Logging/Logger.h"

typedef void(__fastcall* fnSetPlaybackRate)(double rate);
fnSetPlaybackRate oSetPlaybackRate;
void __fastcall SetPlaybackRateHook(double rate)
{
	oSetPlaybackRate(100);
}

CinnamonResult Hooks::installManagedHook(std::string name, Method method, LPVOID pDetour, LPVOID* ppOriginal, HookType hookType)
{
	method.Compile();
	void* functionAddress = method.GetNativeStart();

	return Cinnamon::InstallHook(name, functionAddress, pDetour, ppOriginal, hookType);
}

void Hooks::InstallAllHooks()
{
	if (ConfigManager::CompatibilityContext())
	{
		uintptr_t pEndScene = Vanilla::FindSignature("\x6A\x14\xB8\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x8B\x75\x08\x8B\xDE\xF7\xDB", "xxx????x????xxxxxxx", reinterpret_cast<uintptr_t>(GetModuleHandleA("d3d9.dll")), Vanilla::GetModuleSize("d3d9.dll"));
		if (pEndScene)
		{
			if (Cinnamon::InstallHook("EndScene", reinterpret_cast<void*>(pEndScene), endSceneHook, reinterpret_cast<void**>(&oEndScene)) == CinnamonResult::Success)
				Logger::Log(LogSeverity::Info, "Hooked EndScene");
			else
				Logger::Log(LogSeverity::Error, "Failed to hook EndScene");
		}
		else
			Logger::Log(LogSeverity::Error, "Failed to hook EndScene");
	}
	else
	{
		void* pWglSwapBuffers = GetProcAddress(GetModuleHandleA("opengl32.dll"), "wglSwapBuffers");
		if (Cinnamon::InstallHook("SwapBuffers", pWglSwapBuffers, wglSwapBuffersHook, reinterpret_cast<LPVOID*>(&oWglSwapBuffers)) == CinnamonResult::Success)
			Logger::Log(LogSeverity::Info, "Hooked wglSwapBuffers");
		else
			Logger::Log(LogSeverity::Error, "Failed to hook wglSwapBuffers");
	}

	void* pGetKeyboardState = GetProcAddress(GetModuleHandleA("user32.dll"), "GetKeyboardState");
	if (Cinnamon::InstallHook("GetKeyboardState", pGetKeyboardState, Overlay::HandleKeyboardInputHook, reinterpret_cast<LPVOID*>(&Overlay::oHandleKeyboardInput)) == CinnamonResult::Success)
		Logger::Log(LogSeverity::Info, "Hooked GetKeyboardState");
	else
		Logger::Log(LogSeverity::Error, "Failed to hook GetKeyboardState");

	if (installManagedHook("SetPlaybackRate", Vanilla::Explorer["osu.Audio.AudioEngine"]["set_CurrentPlaybackRate"].Method, SetPlaybackRateHook, reinterpret_cast<LPVOID*>(&oSetPlaybackRate)) == CinnamonResult::Success)
		Logger::Log(LogSeverity::Info, "Hooked SetPlaybackRate");
	else
		Logger::Log(LogSeverity::Error, "Failed to hook SetPlaybackRate");
}

void Hooks::UninstallAllHooks()
{
	Cinnamon::UninstallAllHooks();
}
