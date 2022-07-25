#include "DiscordRPC.h"

#include "Vanilla.h"

#include "../Memory.h"
#include "../../Config/Config.h"

void __fastcall DiscordRPC::updateStatusHook(void* instance, int mode, int status, CLRString* beatmapDetails)
{
	oUpdateStatus(instance, Config::Misc::DiscordRichPresenceSpoofer::Enabled && Config::Misc::DiscordRichPresenceSpoofer::CustomPlayModeEnabled ? Config::Misc::DiscordRichPresenceSpoofer::CustomPlayMode : mode, status, beatmapDetails);
}

void __fastcall DiscordRPC::updateMatchHook(void* instance, void* match)
{
	oUpdateMatch(instance, Config::Misc::DiscordRichPresenceSpoofer::Enabled && Config::Misc::DiscordRichPresenceSpoofer::HideMatchButton ? nullptr : match);
}

void __fastcall DiscordRPC::setLargeImageTextHook(void* instance, CLRString* string)
{
	if (Config::Misc::DiscordRichPresenceSpoofer::Enabled && Config::Misc::DiscordRichPresenceSpoofer::CustomLargeImageTextEnabled)
	{
		wchar_t buf[128];
		swprintf_s(buf, 128, L"%hs", Config::Misc::DiscordRichPresenceSpoofer::CustomLargeImageText);

		oSetLargeImageText(instance, Vanilla::AllocateCLRString(buf));
	}
	else oSetLargeImageText(instance, string);
}

void __fastcall DiscordRPC::setStateHook(void* instance, CLRString* string)
{
	if (Config::Misc::DiscordRichPresenceSpoofer::Enabled && Config::Misc::DiscordRichPresenceSpoofer::CustomStateEnabled)
	{
		wchar_t buf[128];
		swprintf_s(buf, 128, L"%hs", Config::Misc::DiscordRichPresenceSpoofer::CustomState);

		oSetState(instance, Vanilla::AllocateCLRString(buf));
	}
	else oSetState(instance, string);
}

void __fastcall DiscordRPC::setDetailsHook(void* instance, CLRString* string)
{
	if (Config::Misc::DiscordRichPresenceSpoofer::Enabled && Config::Misc::DiscordRichPresenceSpoofer::CustomDetailsEnabled)
	{
		wchar_t buf[128];
		swprintf_s(buf, 128, L"%hs", Config::Misc::DiscordRichPresenceSpoofer::CustomDetails);

		oSetDetails(instance, Vanilla::AllocateCLRString(buf));
	}
	else oSetDetails(instance, string);
}

void __fastcall DiscordRPC::setSpectateSecretHook(void* instance, CLRString* string)
{
	oSetSpectateSecret(instance, Config::Misc::DiscordRichPresenceSpoofer::Enabled && Config::Misc::DiscordRichPresenceSpoofer::HideSpectateButton ? nullptr : string);
}

void DiscordRPC::Initialize()
{
	Memory::AddObject("DiscordRPC::UpdateStatus", "55 8B EC 57 56 53 81 EC ?? ?? ?? ?? 8B F1 8D BD ?? ?? ?? ?? B9 ?? ?? ?? ?? 33 C0 F3 AB 8B CE 89 55 DC 8B F1");
	Memory::AddHook("DiscordRPC::UpdateStatus", "DiscordRPC::UpdateStatus", reinterpret_cast<uintptr_t>(updateStatusHook), reinterpret_cast<uintptr_t*>(&oUpdateStatus));

	Memory::AddObject("DiscordRPC::UpdateMatch", "55 8B EC 57 56 53 83 EC 6C 8B F1 8D 7D 9C");
	Memory::AddHook("DiscordRPC::UpdateMatch", "DiscordRPC::UpdateMatch", reinterpret_cast<uintptr_t>(updateMatchHook), reinterpret_cast<uintptr_t*>(&oUpdateMatch));

	Memory::AddObject("DiscordRPC::SetLargeImageText", "8B 55 C0 E8 ?? ?? ?? ?? 8B D0 8B CB 39 09 FF 15", 0x10, 2);
	Memory::AddHook("DiscordRPC::SetLargeImageText", "DiscordRPC::SetLargeImageText", reinterpret_cast<uintptr_t>(setLargeImageTextHook), reinterpret_cast<uintptr_t*>(&oSetLargeImageText));

	Memory::AddObject("DiscordRPC::SetState", "8B D0 8B CF 39 09 FF 15 ?? ?? ?? ?? E9 ?? ?? ?? ?? 8B 7E 08", 0x8, 2);
	Memory::AddHook("DiscordRPC::SetState", "DiscordRPC::SetState", reinterpret_cast<uintptr_t>(setStateHook), reinterpret_cast<uintptr_t*>(&oSetState));

	Memory::AddObject("DiscordRPC::SetDetails", "8B CE 89 55 DC 8B F1 8B 4E 08 8B 15 ?? ?? ?? ?? 39 09 FF 15", 0x14, 2);
	Memory::AddHook("DiscordRPC::SetDetails", "DiscordRPC::SetDetails", reinterpret_cast<uintptr_t>(setDetailsHook), reinterpret_cast<uintptr_t*>(&oSetDetails));

	Memory::AddObject("DiscordRPC::SetSpectateSecret", "39 09 FF 15 ?? ?? ?? ?? 8B C8 33 D2 39 09 FF 15", 0x10, 2);
	Memory::AddHook("DiscordRPC::SetSpectateSecret", "DiscordRPC::SetSpectateSecret", reinterpret_cast<uintptr_t>(setSpectateSecretHook), reinterpret_cast<uintptr_t*>(&oSetSpectateSecret));
}
