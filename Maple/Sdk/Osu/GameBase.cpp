#include "GameBase.h"

#include "ThemidaSDK.h"

#include "Vanilla.h"
#include "Utilities/MemoryUtilities.h"
#include "Math/sRectangle.h"
#include "../../Utilities/Security/xorstr.hpp"

#include "../Memory.h"
#include "../Helpers/Obfuscated.h"

void GameBase::Initialize()
{
	STR_ENCRYPT_START

	Memory::AddObject(xor ("GameBase::Time"), xor ("80 3D ?? ?? ?? ?? 00 74 1B A1 ?? ?? ?? ?? 2B 05 ?? ?? ?? ?? 3D"), 0xA, 1);
	Memory::AddObject(xor ("GameBase::Mode"), xor ("80 B8 ?? ?? ?? ?? 00 75 19 A1 ?? ?? ?? ?? 83 F8 0B 74 0B"), 0xA, 1);

	Memory::AddObject(xor ("GameBase::UpdateTiming"), xor ("8B F1 8B 0D ?? ?? ?? ?? 33 D2 39 09 FF 15 ?? ?? ?? ?? 8B CE FF 15"), 0x16, 2);
	Memory::AddPatch(xor ("GameBase::UpdateTiming_TickratePatch_1"), xor ("GameBase::UpdateTiming"), xor ("DD 05 ?? ?? ?? ?? DC 25 ?? ?? ?? ?? D9 C0 DD 05"), 0x28E, 0x10, MemoryUtilities::IntToByteArray(reinterpret_cast<int>(&tickrate)));
	Memory::AddPatch(xor ("GameBase::UpdateTiming_TickratePatch_2"), xor ("GameBase::UpdateTiming"), xor ("1D ?? ?? ?? ?? DD 05 ?? ?? ?? ?? DC 25"), 0x28E, 0xD, MemoryUtilities::IntToByteArray(reinterpret_cast<int>(&tickrate)));
	Memory::AddPatch(xor ("GameBase::UpdateTiming_TickratePatch_3"), xor ("GameBase::UpdateTiming"), xor ("DD 05 ?? ?? ?? ?? DD 05 ?? ?? ?? ?? DC 25"), 0x28E, 0xE, MemoryUtilities::IntToByteArray(reinterpret_cast<int>(&tickrate)));
	Memory::AddPatch(xor ("GameBase::UpdateTiming_TickratePatch_4"), xor ("GameBase::UpdateTiming"), xor ("DD 1D ?? ?? ?? ?? DD 05 ?? ?? ?? ?? DC 35"), 0x28E, 0xE, MemoryUtilities::IntToByteArray(reinterpret_cast<int>(&tickrate)));

	Memory::AddObject(xor ("GameBase::IsFullscreen"), xor ("55 8B EC 57 56 53 8B F1 80 3D ?? ?? ?? ?? 00 75 05 E9 ?? ?? ?? ?? 81 3D ?? ?? ?? ?? ?? ?? ?? ?? 7C 06 80 7E"), 0xA, 1);
	Memory::AddObject(xor ("GameBase::ClientBounds"), xor ("56 FF 75 F0 8B 15 ?? ?? ?? ?? 83 C2 04 39 09"), 0x6, 1);

	Memory::AddObject(xor ("GameBase::ClientHash"), xor ("EB 0A B9 ?? ?? ?? ?? E8 ?? ?? ?? ?? 50 8B CF BA ?? ?? ?? ?? E8 ?? ?? ?? ?? FF 35 ?? ?? ?? ?? 8B CF BA"), 0x1B, 1);
	Memory::AddObject(xor ("GameBase::UniqueID"), xor ("8D 4D D0 E8 ?? ?? ?? ?? 8B 35 ?? ?? ?? ?? 8B 0D ?? ?? ?? ?? 8B D0 8B 01"), 0xA, 1);
	Memory::AddObject(xor ("GameBase::UniqueID2"), xor ("EB 10 E8 ?? ?? ?? ?? 89 85 ?? ?? ?? ?? E8 ?? ?? ?? ?? 8B 1D ?? ?? ?? ?? B9 ?? ?? ?? ?? E8 ?? ?? ?? ?? 8B F8 B9"), 0x14, 1);
	Memory::AddObject(xor ("GameBase::UniqueCheck"), xor ("C6 40 14 00 8B 1D ?? ?? ?? ?? 8B 0D ?? ?? ?? ?? BA ?? ?? ?? ?? FF 15"), 0x6, 1);

	STR_ENCRYPT_END
}

int GameBase::GetTime()
{
	const uintptr_t timeAddress = Memory::Objects[xor ("GameBase::Time")];

	return timeAddress ? *reinterpret_cast<int*>(timeAddress) : 0;
}

OsuModes GameBase::GetMode()
{
	const uintptr_t modeAddress = Memory::Objects[xor ("GameBase::Mode")];

	return modeAddress ? *reinterpret_cast<OsuModes*>(modeAddress) : OsuModes::Menu;
}

void GameBase::SetTickrate(double value)
{
	tickrate = value;
}

bool GameBase::GetIsFullscreen()
{
	const uintptr_t isFullscreenAddress = Memory::Objects[xor ("GameBase::IsFullscreen")];
	
	return isFullscreenAddress ? *reinterpret_cast<bool*>(isFullscreenAddress) : false;
}

Vector2 GameBase::GetClientSize()
{
	if (const uintptr_t clientBoundsAddress = Memory::Objects[xor ("GameBase::ClientBounds")])
	{
		const sRectangle* clientBounds = *reinterpret_cast<sRectangle**>(clientBoundsAddress);
		
		return { static_cast<float>(clientBounds->Width), static_cast<float>(clientBounds->Height) };
	}
	
	return { 0.f, 0.f };
}

Vector2 GameBase::GetClientPosition()
{
	if (!GetIsFullscreen())
		return { 0.f, 0.f };

	if (const uintptr_t clientBoundsAddress = Memory::Objects[xor ("GameBase::ClientBounds")])
	{
		const sRectangle* clientBounds = *reinterpret_cast<sRectangle**>(clientBoundsAddress);
		
		return { static_cast<float>(clientBounds->X), static_cast<float>(clientBounds->Y) };
	}

	return { 0.f, 0.f };
}

std::wstring GameBase::GetClientHash()
{
	const uintptr_t clientHashAddress = Memory::Objects[xor ("GameBase::ClientHash")];

	return clientHashAddress ? (*reinterpret_cast<CLRString**>(clientHashAddress))->Data().data() : L"";
}

uintptr_t GameBase::GetUniqueIDInstance()
{
	const uintptr_t uniqueIDAddress = Memory::Objects[xor ("GameBase::UniqueID")];

	return uniqueIDAddress ? *reinterpret_cast<uintptr_t*>(uniqueIDAddress) : 0u;
}

std::wstring GameBase::GetUniqueID()
{
	const uintptr_t uniqueIDAddress = Memory::Objects[xor ("GameBase::UniqueID")];

	return uniqueIDAddress ? Obfuscated::GetString(*reinterpret_cast<uintptr_t*>(uniqueIDAddress))->Data().data() : L"";
}

uintptr_t GameBase::GetUniqueID2Instance()
{
	const uintptr_t uniqueID2Address = Memory::Objects[xor ("GameBase::UniqueID2")];

	return uniqueID2Address ? *reinterpret_cast<uintptr_t*>(uniqueID2Address) : 0u;
}

std::wstring GameBase::GetUniqueID2()
{
	const uintptr_t uniqueID2Address = Memory::Objects[xor ("GameBase::UniqueID2")];

	return uniqueID2Address ? Obfuscated::GetString(*reinterpret_cast<uintptr_t*>(uniqueID2Address))->Data().data() : L"";
}

uintptr_t GameBase::GetUniqueCheckInstance()
{
	const uintptr_t uniqueCheckAddress = Memory::Objects[xor ("GameBase::UniqueCheck")];

	return uniqueCheckAddress ? *reinterpret_cast<uintptr_t*>(uniqueCheckAddress) : 0u;
}

std::wstring GameBase::GetUniqueCheck()
{
	const uintptr_t uniqueCheckAddress = Memory::Objects[xor ("GameBase::UniqueCheck")];

	return uniqueCheckAddress ? Obfuscated::GetString(*reinterpret_cast<uintptr_t*>(uniqueCheckAddress))->Data().data() : L"";
}
