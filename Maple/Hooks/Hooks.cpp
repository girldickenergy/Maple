#include "Hooks.h"

#include <Cinnamon.h>

#include "../Features/Misc/RichPresence.h"
#include "../Features/Misc/SpectateHandler.h"
#include "../Features/Timewarp/Timewarp.h"
#include "../Features/Visuals/VisualsSpoofers.h"
#include "../Sdk/ConfigManager/ConfigManager.h"
#include "../UI/Overlay.h"
#include "../Utilities/Logging/Logger.h"

CinnamonResult Hooks::installManagedHook(std::string name, Method method, LPVOID pDetour, LPVOID* ppOriginal, HookType hookType)
{
	method.Compile();
	void* functionAddress = method.GetNativeStart();

	return Cinnamon::InstallHook(name, functionAddress, pDetour, ppOriginal, hookType);
}

void Hooks::InstallAllHooks()
{
	if (installManagedHook("AudioTrackBass.GetPlaybackRate", Vanilla::Explorer["osu.Audio.AudioTrackBass"]["get_PlaybackRate"].Method, Timewarp::AudioTrackBass_GetPlaybackRateHook, reinterpret_cast<LPVOID*>(&Timewarp::oAudioTrackBass_GetPlaybackRate)) == CinnamonResult::Success)
		Logger::Log(LogSeverity::Info, "Hooked AudioTrackBass.GetPlaybackRate");
	else
		Logger::Log(LogSeverity::Error, "Failed to hook AudioTrackBass.GetPlaybackRate");

	if (installManagedHook("AudioTrackVirtual.GetPlaybackRate", Vanilla::Explorer["osu.Audio.AudioTrackVirtual"]["get_PlaybackRate"].Method, Timewarp::AudioTrackVirtual_GetPlaybackRateHook, reinterpret_cast<LPVOID*>(&Timewarp::oAudioTrackVirtual_GetPlaybackRate)) == CinnamonResult::Success)
		Logger::Log(LogSeverity::Info, "Hooked AudioTrackVirtual.GetPlaybackRate");
	else
		Logger::Log(LogSeverity::Error, "Failed to hook AudioTrackVirtual.GetPlaybackRate");

	if (installManagedHook("AddParameter", Vanilla::Explorer["osu_common.Helpers.pWebRequest"]["AddParameter"].Method, Timewarp::AddParameterHook, reinterpret_cast<LPVOID*>(&Timewarp::oAddParameter)) == CinnamonResult::Success)
		Logger::Log(LogSeverity::Info, "Hooked AddParameter");
	else
		Logger::Log(LogSeverity::Error, "Failed to hook AddParameter");

	if (installManagedHook("Parse", Vanilla::Explorer["osu.GameplayElements.HitObjectManager"]["parse"].Method, VisualsSpoofers::ParseHook, reinterpret_cast<LPVOID*>(&VisualsSpoofers::oParse)) == CinnamonResult::Success)
		Logger::Log(LogSeverity::Info, "Hooked Parse");
	else
		Logger::Log(LogSeverity::Error, "Failed to hook Parse");

	if (installManagedHook("ApplyStacking", Vanilla::Explorer["osu.GameplayElements.HitObjectManager"]["ApplyStacking"].Method, VisualsSpoofers::ApplyStackingHook, reinterpret_cast<LPVOID*>(&VisualsSpoofers::oApplyStacking)) == CinnamonResult::Success)
		Logger::Log(LogSeverity::Info, "Hooked ApplyStacking");
	else
		Logger::Log(LogSeverity::Error, "Failed to hook ApplyStacking");

	if (installManagedHook("AddFollowPoints", Vanilla::Explorer["osu.GameplayElements.HitObjectManager"]["AddFollowPoints"].Method, VisualsSpoofers::AddFollowPointsHook, reinterpret_cast<LPVOID*>(&VisualsSpoofers::oAddFollowPoints)) == CinnamonResult::Success)
		Logger::Log(LogSeverity::Info, "Hooked AddFollowPoints");
	else
		Logger::Log(LogSeverity::Error, "Failed to hook AddFollowPoints");

	if (installManagedHook("PushNewFrame", Vanilla::Explorer["osu.Online.StreamingManager"]["PushNewFrame"].Method, SpectateHandler::PushNewFrameHook, reinterpret_cast<LPVOID*>(&SpectateHandler::oPushNewFrame)) == CinnamonResult::Success)
		Logger::Log(LogSeverity::Info, "Hooked PushNewFrame");
	else
		Logger::Log(LogSeverity::Error, "Failed to hook PushNewFrame");

	if (installManagedHook("PurgeFrames", Vanilla::Explorer["osu.Online.StreamingManager"]["PurgeFrames"].Method, SpectateHandler::PurgeFramesHook, reinterpret_cast<LPVOID*>(&SpectateHandler::oPurgeFrames)) == CinnamonResult::Success)
		Logger::Log(LogSeverity::Info, "Hooked PurgeFrames");
	else
		Logger::Log(LogSeverity::Error, "Failed to hook PurgeFrames");

	if (installManagedHook("UpdateStatus", Vanilla::Explorer["DiscordRPC.Assets"]["set_LargeImageText"].Method, RichPresence::SetLargeImageTextHook, reinterpret_cast<LPVOID*>(&RichPresence::oSetLargeImageText)) == CinnamonResult::Success)
		Logger::Log(LogSeverity::Info, "Hooked set_LargeImageText");
	else
		Logger::Log(LogSeverity::Error, "Failed to hook set_LargeImageText");

	void* pGetKeyboardState = GetProcAddress(GetModuleHandleA("user32.dll"), "GetKeyboardState");
	if (Cinnamon::InstallHook("GetKeyboardState", pGetKeyboardState, Overlay::HandleKeyboardInputHook, reinterpret_cast<LPVOID*>(&Overlay::oHandleKeyboardInput)) == CinnamonResult::Success)
		Logger::Log(LogSeverity::Info, "Hooked GetKeyboardState");
	else
		Logger::Log(LogSeverity::Error, "Failed to hook GetKeyboardState");

	//hook swapbuffers/endscene AFTER everything else. make menu inaccessible for as long as possible because hooking of other functions can take a while with themida.
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
}

void Hooks::UninstallAllHooks()
{
	Cinnamon::UninstallAllHooks();
}
