#include "BanchoClient.h"

#include <iostream>

#include "../Memory.h"
#include "Utilities/MemoryUtilities.h"
#include "../../Features/Spoofer/Spoofer.h"

void BanchoClient::Initialize()
{
	Memory::AddObject("BanchoClient::InitializePrivate", "55 8B EC 57 56 53 81 EC ?? ?? ?? ?? 8D BD ?? ?? ?? ?? B9 ?? ?? ?? ?? 33 C0 F3 AB 8B 05");

	Memory::AddPatch("BanchoClient::InitializePrivate_ClientHashPatch_1", "BanchoClient::InitializePrivate", "BA ?? ?? ?? ?? E8 ?? ?? ?? ?? 8B 8D ?? ?? ?? ?? E8", 0xC7A, 0xA, { 0xB8 });
	Memory::AddPatch("BanchoClient::InitializePrivate_ClientHashPatch_2", "BanchoClient::InitializePrivate", "BA ?? ?? ?? ?? E8 ?? ?? ?? ?? B8 8D ?? ?? ?? ?? E8", 0xC7A, 0xB, MemoryUtilities::IntToByteArray(reinterpret_cast<int>(Spoofer::GetClientHash)));
	Memory::AddPatch("BanchoClient::InitializePrivate_ClientHashPatch_3", "BanchoClient::InitializePrivate", "BA ?? ?? ?? ?? E8 ?? ?? ?? ?? B8 ?? ?? ?? ?? ?? E8", 0xC7A, 0xF, { 0xFF, 0xD0, 0x90, 0x90, 0x90, 0x90 });
}

void BanchoClient::InitializePrivate()
{
	if (const uintptr_t initializePrivateAddress = Memory::Objects["BanchoClient::InitializePrivate"])
	{
		const fnInitializePrivate initializePrivate = reinterpret_cast<fnInitializePrivate>(initializePrivateAddress);
		initializePrivate();
	}
}
