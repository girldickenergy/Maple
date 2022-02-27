#include "Hooks.h"

#include <Cinnamon.h>

#include "../Communication/Communication.h"
#include "../Features/AimAssist/AimAssist.h"
#include "../Features/Misc/RichPresence.h"
#include "../Features/Misc/ScoreSubmission.h"
#include "../Features/Misc/SpectateHandler.h"
#include "../Features/Relax/Relax.h"
#include "../Features/Timewarp/Timewarp.h"
#include "../Features/Visuals/VisualsSpoofers.h"
#include "../Sdk/Anticheat/Anticheat.h"
#include "../Sdk/ConfigManager/ConfigManager.h"
#include "../Sdk/Player/Player.h"
#include "../UI/Overlay.h"
#include "../Logging/Logger.h"
#include "../Utilities/Security/Security.h"
#include "../Features/Spoofer/Spoofer.h"
#include "ErrorSubmission.h"
#include "AddParameter.h"
#include "Rendering.h"

CinnamonResult Hooks::installManagedHook(std::string name, Method method, LPVOID pDetour, LPVOID* ppOriginal, HookType hookType)
{
	VM_SHARK_BLACK_START
	method.Compile();
	void* functionAddress = method.GetNativeStart();

	CinnamonResult result = Cinnamon::InstallHook(name, functionAddress, pDetour, ppOriginal, hookType);
	VM_SHARK_BLACK_END
	return result;
}

void Hooks::InstallAllHooks()
{
	VM_SHARK_BLACK_START
	STR_ENCRYPT_START
	
	if (!Communication::EstablishedConnection || !Communication::HeartbeatThreadLaunched || !Communication::HandshakeSucceeded)
		Security::CorruptMemory();

	if (installManagedHook("SubmitError", Vanilla::Explorer["osu.Helpers.ErrorSubmission"]["Submit"].Method, ErrorSubmission::SubmitErrorHook, reinterpret_cast<LPVOID*>(&ErrorSubmission::oSubmitError)) == CinnamonResult::Success)
		Logger::Log(LogSeverity::Info, "Hooked SubmitError");
	else
		Logger::Log(LogSeverity::Error, "Failed to hook SubmitError");

	if (installManagedHook("sendCurrentTrack", Vanilla::Explorer["osu.Helpers.Scrobbler"]["sendCurrentTrack"].Method, Anticheat::SendCurrentTrackHook, reinterpret_cast<LPVOID*>(&Anticheat::oSendCurrentTrack)) == CinnamonResult::Success)
		Logger::Log(LogSeverity::Info, "Hooked sendCurrentTrack");
	else
		Logger::Log(LogSeverity::Error, "Failed to hook sendCurrentTrack");
	
	//TODO: causing weird access violations only in debug
	#ifndef _DEBUG
		if (installManagedHook("get_CurrentPlaybackRate", Vanilla::Explorer["osu.Audio.AudioEngine"]["get_CurrentPlaybackRate"].Method, Timewarp::GetCurrentPlaybackRateHook, reinterpret_cast<LPVOID*>(&Timewarp::oGetCurrentPlaybackRate), HookType::UndetectedByteCodePatch) == CinnamonResult::Success)
			Logger::Log(LogSeverity::Info, "Hooked get_CurrentPlaybackRate");
		else
			Logger::Log(LogSeverity::Error, "Failed to hook get_CurrentPlaybackRate");
	#endif
	
	if (installManagedHook("set_CurrentPlaybackRate", Vanilla::Explorer["osu.Audio.AudioEngine"]["set_CurrentPlaybackRate"].Method, Timewarp::SetCurrentPlaybackRateHook, reinterpret_cast<LPVOID*>(&Timewarp::oSetCurrentPlaybackRate)) == CinnamonResult::Success)
		Logger::Log(LogSeverity::Info, "Hooked set_CurrentPlaybackRate");
	else
		Logger::Log(LogSeverity::Error, "Failed to hook set_CurrentPlaybackRate");

	if (installManagedHook("AddParameter", Vanilla::Explorer["osu_common.Helpers.pWebRequest"]["AddParameter"].Method, AddParameter::AddParameterHook, reinterpret_cast<LPVOID*>(&AddParameter::oAddParameter)) == CinnamonResult::Success)
		Logger::Log(LogSeverity::Info, "Hooked AddParameter");
	else
		Logger::Log(LogSeverity::Error, "Failed to hook AddParameter");

	if (installManagedHook("Parse", Vanilla::Explorer["osu.GameplayElements.HitObjectManager"]["parse"].Method, VisualsSpoofers::ParseHook, reinterpret_cast<LPVOID*>(&VisualsSpoofers::oParse)) == CinnamonResult::Success)
		Logger::Log(LogSeverity::Info, "Hooked Parse");
	else
		Logger::Log(LogSeverity::Error, "Failed to hook Parse");

	if (installManagedHook("UpdateStacking", Vanilla::Explorer["osu.GameplayElements.HitObjectManager"]["UpdateStacking"].Method, VisualsSpoofers::UpdateStackingHook, reinterpret_cast<LPVOID*>(&VisualsSpoofers::oUpdateStacking)) == CinnamonResult::Success)
		Logger::Log(LogSeverity::Info, "Hooked UpdateStacking");
	else
		Logger::Log(LogSeverity::Error, "Failed to hook UpdateStacking");

	if (installManagedHook("ApplyOldStacking", Vanilla::Explorer["osu.GameplayElements.HitObjectManager"]["ApplyOldStacking"].Method, VisualsSpoofers::ApplyOldStackingHook, reinterpret_cast<LPVOID*>(&VisualsSpoofers::oApplyOldStacking)) == CinnamonResult::Success)
		Logger::Log(LogSeverity::Info, "Hooked ApplyOldStacking");
	else
		Logger::Log(LogSeverity::Error, "Failed to hook ApplyOldStacking");

	if (installManagedHook("AddFollowPoints", Vanilla::Explorer["osu.GameplayElements.HitObjectManager"]["AddFollowPoints"].Method, VisualsSpoofers::AddFollowPointsHook, reinterpret_cast<LPVOID*>(&VisualsSpoofers::oAddFollowPoints)) == CinnamonResult::Success)
		Logger::Log(LogSeverity::Info, "Hooked AddFollowPoints");
	else
		Logger::Log(LogSeverity::Error, "Failed to hook AddFollowPoints");

	if (installManagedHook("LoadFlashlight", Vanilla::Explorer["osu.GameModes.Play.Rulesets.Ruleset"]["loadFlashlight"].Method, VisualsSpoofers::LoadFlashlightHook, reinterpret_cast<LPVOID*>(&VisualsSpoofers::oLoadFlashlight)) == CinnamonResult::Success)
		Logger::Log(LogSeverity::Info, "Hooked LoadFlashlight");
	else
		Logger::Log(LogSeverity::Error, "Failed to hook LoadFlashlight");
	
	if (installManagedHook("LoadFlashlightMania", Vanilla::Explorer["osu.GameModes.Play.Rulesets.Mania.RulesetMania"]["loadFlashlight"].Method, VisualsSpoofers::LoadFlashlightManiaHook, reinterpret_cast<LPVOID*>(&VisualsSpoofers::oLoadFlashlightMania)) == CinnamonResult::Success)
		Logger::Log(LogSeverity::Info, "Hooked LoadFlashlightMania");
	else
		Logger::Log(LogSeverity::Error, "Failed to hook LoadFlashlightMania");

	if (installManagedHook("UpdateFlashlight", Vanilla::Explorer["osu.GameModes.Play.Player"]["UpdateFlashlight"].Method, VisualsSpoofers::UpdateFlashlightHook, reinterpret_cast<LPVOID*>(&VisualsSpoofers::oUpdateFlashlight)) == CinnamonResult::Success)
		Logger::Log(LogSeverity::Info, "Hooked UpdateFlashlight");
	else
		Logger::Log(LogSeverity::Error, "Failed to hook UpdateFlashlight");

	if (installManagedHook("HasHiddenSprites", Vanilla::Explorer["osu.GameModes.Play.Rulesets.Mania.StageMania"]["get_hasHiddenSprites"].Method, VisualsSpoofers::HasHiddenSpritesHook, reinterpret_cast<LPVOID*>(&VisualsSpoofers::oHasHiddenSprites)) == CinnamonResult::Success)
		Logger::Log(LogSeverity::Info, "Hooked HasHiddenSprites");
	else
		Logger::Log(LogSeverity::Error, "Failed to hook HasHiddenSprites");

	if (installManagedHook("PushNewFrame", Vanilla::Explorer["osu.Online.StreamingManager"]["PushNewFrame"].Method, SpectateHandler::PushNewFrameHook, reinterpret_cast<LPVOID*>(&SpectateHandler::oPushNewFrame)) == CinnamonResult::Success)
		Logger::Log(LogSeverity::Info, "Hooked PushNewFrame");
	else
		Logger::Log(LogSeverity::Error, "Failed to hook PushNewFrame");

	if (installManagedHook("PurgeFrames", Vanilla::Explorer["osu.Online.StreamingManager"]["PurgeFrames"].Method, SpectateHandler::PurgeFramesHook, reinterpret_cast<LPVOID*>(&SpectateHandler::oPurgeFrames), HookType::UndetectedByteCodePatch) == CinnamonResult::Success)
		Logger::Log(LogSeverity::Info, "Hooked PurgeFrames");
	else
		Logger::Log(LogSeverity::Error, "Failed to hook PurgeFrames");

	if (installManagedHook("set_LargeImageText", Vanilla::Explorer["DiscordRPC.Assets"]["set_LargeImageText"].Method, RichPresence::SetLargeImageTextHook, reinterpret_cast<LPVOID*>(&RichPresence::oSetLargeImageText)) == CinnamonResult::Success)
		Logger::Log(LogSeverity::Info, "Hooked set_LargeImageText");
	else
		Logger::Log(LogSeverity::Error, "Failed to hook set_LargeImageText");

	if (installManagedHook("Submit", Vanilla::Explorer["osu.GameplayElements.Scoring.Score"]["Submit"].Method, ScoreSubmission::SubmitHook, reinterpret_cast<LPVOID*>(&ScoreSubmission::oSubmit)) == CinnamonResult::Success)
		Logger::Log(LogSeverity::Info, "Hooked Submit");
	else
		Logger::Log(LogSeverity::Error, "Failed to hook Submit");

	if (installManagedHook("get_Item", Vanilla::Explorer["Microsoft.Xna.Framework.Input.KeyboardState"]["get_Item"].Method, Relax::UpdateKeyboardInput, reinterpret_cast<LPVOID*>(&Relax::oUpdateKeyboardInput)) == CinnamonResult::Success)
		Logger::Log(LogSeverity::Info, "Hooked get_Item");
	else
		Logger::Log(LogSeverity::Error, "Failed to hook get_Item");

	if (installManagedHook("Initialize", Vanilla::Explorer["osu.GameModes.Play.Player"]["Initialize"].Method, Player::PlayerInitialize, reinterpret_cast<LPVOID*>(&Player::oPlayerInitialize)) == CinnamonResult::Success)
		Logger::Log(LogSeverity::Info, "Hooked Initialize");
	else
		Logger::Log(LogSeverity::Error, "Failed to hook Initialize");

	if (installManagedHook("OnLoadComplete", Vanilla::Explorer["osu.GameModes.Play.Player"]["OnLoadComplete"].Method, Player::OnPlayerLoadCompleteHook, reinterpret_cast<LPVOID*>(&Player::oOnPlayerLoadComplete)) == CinnamonResult::Success)
		Logger::Log(LogSeverity::Info, "Hooked OnLoadComplete");
	else
		Logger::Log(LogSeverity::Error, "Failed to hook OnLoadComplete");

	if (installManagedHook("set_MousePosition", Vanilla::Explorer["osu.Input.Handlers.MouseManager"]["set_MousePosition"].Method, AimAssist::UpdateCursorPosition, reinterpret_cast<LPVOID*>(&AimAssist::oUpdateCursorPosition), HookType::UndetectedByteCodePatch) == CinnamonResult::Success)
		Logger::Log(LogSeverity::Info, "Hooked set_MousePosition");
	else
		Logger::Log(LogSeverity::Error, "Failed to hook set_MousePosition");

	TypeExplorer obfuscatedStringType = Vanilla::Explorer["osu.GameBase"]["UniqueId"].Field.GetTypeUnsafe();
	if (installManagedHook("ObfuscatedStringGetValue", obfuscatedStringType["get_Value"].Method, Spoofer::ObfuscatedStringGetValueHook, reinterpret_cast<LPVOID*>(&Spoofer::oObfuscatedStringGetValue)) == CinnamonResult::Success)
		Logger::Log(LogSeverity::Info, "Hooked ObfuscatedStringGetValue");
	else
		Logger::Log(LogSeverity::Error, "Failed to hook ObfuscatedStringGetValue");

	if (installManagedHook("ObfuscatedStringSetValue", obfuscatedStringType["set_Value"].Method, Spoofer::ObfuscatedStringSetValueHook, reinterpret_cast<LPVOID*>(&Spoofer::oObfuscatedStringSetValue)) == CinnamonResult::Success)
		Logger::Log(LogSeverity::Info, "Hooked ObfuscatedStringSetValue");
	else
		Logger::Log(LogSeverity::Error, "Failed to hook ObfuscatedStringSetValue");

	void* pGetKeyboardState = GetProcAddress(GetModuleHandleA("user32.dll"), "GetKeyboardState");
	if (Cinnamon::InstallHook("GetKeyboardState", pGetKeyboardState, Overlay::HandleKeyboardInputHook, reinterpret_cast<LPVOID*>(&Overlay::oHandleKeyboardInput)) == CinnamonResult::Success)
		Logger::Log(LogSeverity::Info, "Hooked GetKeyboardState");
	else
		Logger::Log(LogSeverity::Error, "Failed to hook GetKeyboardState");

	//hook swapbuffers/present AFTER everything else. make menu inaccessible for as long as possible because hooking of other functions can take a while with themida.
	if (ConfigManager::CompatibilityContext())
	{
		uintptr_t pPresent = Vanilla::FindSignature("\x8B\xFF\x55\x8B\xEC\xFF\x75\x1C", "xxxxxxxx", reinterpret_cast<uintptr_t>(GetModuleHandleA("d3d9.dll")), Vanilla::GetModuleSize("d3d9.dll"));
		if (pPresent)
		{
			if (Cinnamon::InstallHook("Present", reinterpret_cast<void*>(pPresent), Rendering::PresentHook, reinterpret_cast<void**>(&Rendering::oPresent)) == CinnamonResult::Success)
				Logger::Log(LogSeverity::Info, "Hooked Present");
			else
				Logger::Log(LogSeverity::Error, "Failed to hook Present");
		}
		else 
			Logger::Log(LogSeverity::Error, "Failed to hook Present");
	}
	else
	{
		void* pWglSwapBuffers = GetProcAddress(GetModuleHandleA("opengl32.dll"), "wglSwapBuffers");
		if (Cinnamon::InstallHook("SwapBuffers", pWglSwapBuffers, Rendering::WglSwapBuffersHook, reinterpret_cast<LPVOID*>(&Rendering::oWglSwapBuffers)) == CinnamonResult::Success)
			Logger::Log(LogSeverity::Info, "Hooked wglSwapBuffers");
		else
			Logger::Log(LogSeverity::Error, "Failed to hook wglSwapBuffers");
	}

	STR_ENCRYPT_END
	VM_SHARK_BLACK_END
}

void Hooks::UninstallAllHooks()
{
	Cinnamon::UninstallAllHooks();
}
