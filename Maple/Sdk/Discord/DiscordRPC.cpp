#include "DiscordRPC.h"

#include "ThemidaSDK.h"
#include "Vanilla.h"

#include "../Memory.h"
#include "../../Config/Config.h"
#include "../../Utilities/Security/xorstr.hpp"

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
	STR_ENCRYPT_START

	Memory::AddObject(xor ("DiscordRPC::UpdateStatus"), xor ("55 8B EC 57 56 53 81 EC ?? ?? ?? ?? 8B F1 8D BD ?? ?? ?? ?? B9 ?? ?? ?? ?? 33 C0 F3 AB 8B CE 89 55 DC 8B F1 8B 4E 08 8B 15 ?? ?? ?? ?? 39 09 FF 15 ?? ?? ?? ?? 8B 46 08 8B 40 10"));
	Memory::AddHook(xor ("DiscordRPC::UpdateStatus"), xor ("DiscordRPC::UpdateStatus"), reinterpret_cast<uintptr_t>(updateStatusHook), reinterpret_cast<uintptr_t*>(&oUpdateStatus), VanillaHookType::UndetectedInline);

	Memory::AddObject(xor ("DiscordRPC::UpdateMatch"), xor ("55 8B EC 57 56 53 83 EC 6C 8B F1 8D 7D 9C B9 ?? ?? ?? ?? 33 C0 F3 AB 8B CE 8B F1 8B FA 85 FF 0F 85 ?? ?? ?? ?? 8B 46 08 33 D2"));
	Memory::AddHook(xor ("DiscordRPC::UpdateMatch"), xor ("DiscordRPC::UpdateMatch"), reinterpret_cast<uintptr_t>(updateMatchHook), reinterpret_cast<uintptr_t*>(&oUpdateMatch));

	Memory::AddObject(xor ("DiscordRPC::SetLargeImageText"), xor ("8B 55 C0 E8 ?? ?? ?? ?? 8B D0 8B CB 39 09 FF 15 ?? ?? ?? ?? 8B 46 08 8B 58 10 B9 ?? ?? ?? ?? E8 ?? ?? ?? ?? 8B F8 B9"), 0x10, 2);
	Memory::AddHook(xor ("DiscordRPC::SetLargeImageText"), xor ("DiscordRPC::SetLargeImageText"), reinterpret_cast<uintptr_t>(setLargeImageTextHook), reinterpret_cast<uintptr_t*>(&oSetLargeImageText));

	Memory::AddObject(xor ("DiscordRPC::SetState"), xor ("8B D0 8B CF 39 09 FF 15 ?? ?? ?? ?? E9 ?? ?? ?? ?? 8B 7E 08 B9 ?? ?? ?? ?? E8 ?? ?? ?? ?? 8B D0 8B CF 39 09 FF 15 ?? ?? ?? ?? E9 ?? ?? ?? ?? 8B 7E 08 B9 ?? ?? ?? ?? E8 ?? ?? ?? ?? 8B D0 8B CF 39 09 FF 15 ?? ?? ?? ?? E9 ?? ?? ?? ?? 8B 7E 08 B9 ?? ?? ?? ?? E8 ?? ?? ?? ?? 8B D0 8B CF"), 0x8, 2);
	Memory::AddHook(xor ("DiscordRPC::SetState"), xor ("DiscordRPC::SetState"), reinterpret_cast<uintptr_t>(setStateHook), reinterpret_cast<uintptr_t*>(&oSetState));

	Memory::AddObject(xor ("DiscordRPC::SetDetails"), xor ("8B CE 89 55 DC 8B F1 8B 4E 08 8B 15 ?? ?? ?? ?? 39 09 FF 15 ?? ?? ?? ?? 8B 46 08 8B 40 10 89 45 D4 B9"), 0x14, 2);
	Memory::AddHook(xor ("DiscordRPC::SetDetails"), xor ("DiscordRPC::SetDetails"), reinterpret_cast<uintptr_t>(setDetailsHook), reinterpret_cast<uintptr_t*>(&oSetDetails));

	Memory::AddObject(xor ("DiscordRPC::SetSpectateSecret"), xor ("39 09 FF 15 ?? ?? ?? ?? 8B C8 33 D2 39 09 FF 15 ?? ?? ?? ?? 8B 15 ?? ?? ?? ?? 8B CE FF 15 ?? ?? ?? ?? EB 3F 89 85 ?? ?? ?? ?? 89 45 D8"), 0x10, 2);
	Memory::AddHook(xor ("DiscordRPC::SetSpectateSecret"), xor ("DiscordRPC::SetSpectateSecret"), reinterpret_cast<uintptr_t>(setSpectateSecretHook), reinterpret_cast<uintptr_t*>(&oSetSpectateSecret));

	STR_ENCRYPT_END
}
