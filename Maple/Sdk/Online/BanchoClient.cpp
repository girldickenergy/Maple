#include "BanchoClient.h"

#include "ThemidaSDK.h"

#include "../Memory.h"
#include "Utilities/MemoryUtilities.h"
#include "../../Features/Spoofer/Spoofer.h"
#include "../../Utilities/Security/xorstr.hpp"
#include "../../Communication/Communication.h"

void BanchoClient::Initialize()
{
	VM_FISH_RED_START
	STR_ENCRYPT_START

	Memory::AddObject(xorstr_("BanchoClient::InitializePrivate"), xorstr_("89 45 F8 80 3D ?? ?? ?? ?? 00 75 06 FF 15 ?? ?? ?? ?? FF 15"), 0x14, 2);

	Memory::AddPatch(xorstr_("BanchoClient::InitializePrivate_ClientHashPatch_1"), xorstr_("BanchoClient::InitializePrivate"), xorstr_("BA ?? ?? ?? ?? E8 ?? ?? ?? ?? 8B 8D ?? ?? ?? ?? E8"), 0xC7A, 0xA, { 0xB8 });
	Memory::AddPatch(xorstr_("BanchoClient::InitializePrivate_ClientHashPatch_2"), xorstr_("BanchoClient::InitializePrivate"), xorstr_("BA ?? ?? ?? ?? E8 ?? ?? ?? ?? B8 8D ?? ?? ?? ?? E8"), 0xC7A, 0xB, MemoryUtilities::IntToByteArray(reinterpret_cast<int>(Spoofer::GetClientHash)));
	Memory::AddPatch(xorstr_("BanchoClient::InitializePrivate_ClientHashPatch_3"), xorstr_("BanchoClient::InitializePrivate"), xorstr_("BA ?? ?? ?? ?? E8 ?? ?? ?? ?? B8 ?? ?? ?? ?? ?? E8"), 0xC7A, 0xF, { 0xFF, 0xD0, 0x90, 0x90, 0x90, 0x90 });

	STR_ENCRYPT_END
	VM_FISH_RED_END
}

void BanchoClient::InitializePrivate()
{
	if (const uintptr_t initializePrivateAddress = Memory::Objects[xorstr_("BanchoClient::InitializePrivate")])
	{
		const fnInitializePrivate initializePrivate = reinterpret_cast<fnInitializePrivate>(initializePrivateAddress);
		initializePrivate();
	}
}
