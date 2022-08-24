#include "BanchoClient.h"

#include "ThemidaSDK.h"

#include "../Memory.h"
#include "Utilities/MemoryUtilities.h"
#include "../../Features/Spoofer/Spoofer.h"
#include "../../Utilities/Security/xorstr.hpp"

void BanchoClient::Initialize()
{
	STR_ENCRYPT_START
	
	Memory::AddObject(xor ("BanchoClient::InitializePrivate"), xor ("89 45 F8 80 3D ?? ?? ?? ?? 00 75 06 FF 15 ?? ?? ?? ?? FF 15"), 0x14, 2);

	Memory::AddPatch(xor ("BanchoClient::InitializePrivate_ClientHashPatch_1"), xor ("BanchoClient::InitializePrivate"), xor ("BA ?? ?? ?? ?? E8 ?? ?? ?? ?? 8B 8D ?? ?? ?? ?? E8"), 0xC7A, 0xA, { 0xB8 });
	Memory::AddPatch(xor ("BanchoClient::InitializePrivate_ClientHashPatch_2"), xor ("BanchoClient::InitializePrivate"), xor ("BA ?? ?? ?? ?? E8 ?? ?? ?? ?? B8 8D ?? ?? ?? ?? E8"), 0xC7A, 0xB, MemoryUtilities::IntToByteArray(reinterpret_cast<int>(Spoofer::GetClientHash)));
	Memory::AddPatch(xor ("BanchoClient::InitializePrivate_ClientHashPatch_3"), xor ("BanchoClient::InitializePrivate"), xor ("BA ?? ?? ?? ?? E8 ?? ?? ?? ?? B8 ?? ?? ?? ?? ?? E8"), 0xC7A, 0xF, { 0xFF, 0xD0, 0x90, 0x90, 0x90, 0x90 });

	STR_ENCRYPT_END
}

void BanchoClient::InitializePrivate()
{
	if (const uintptr_t initializePrivateAddress = Memory::Objects[xor ("BanchoClient::InitializePrivate")])
	{
		const fnInitializePrivate initializePrivate = reinterpret_cast<fnInitializePrivate>(initializePrivateAddress);
		initializePrivate();
	}
}
