#include "GameBase.h"

#include "ThemidaSDK.h"

#include "Vanilla.h"
#include "Utilities/MemoryUtilities.h"
#include "Math/sRectangle.h"
#include "../../Utilities/Security/xorstr.hpp"

#include "../Memory.h"

void GameBase::Initialize()
{
	STR_ENCRYPT_START

	Memory::AddObject(xor ("GameBase::Time"), xor ("80 3D ?? ?? ?? ?? 00 74 1B A1 ?? ?? ?? ?? 2B 05 ?? ?? ?? ?? 3D"), 0xA, 1);
	Memory::AddObject(xor ("GameBase::Mode"), xor ("C3 FF 15 ?? ?? ?? ?? 83 3D ?? ?? ?? ?? 02 75 09 80 3D ?? ?? ?? ?? 00 74 70 D9 05"), 0x9, 1);

	Memory::AddObject(xor ("GameBase::UpdateTiming"), xor ("55 8B EC 83 E4 F8 57 56 83 EC 18 8B F9 8B 0D ?? ?? ?? ?? BA ?? ?? ?? ?? 39 09 E8 ?? ?? ?? ?? 8B 0D ?? ?? ?? ?? E8 ?? ?? ?? ?? DD 1C 24 FF 15 ?? ?? ?? ?? D8 3D"));
	Memory::AddPatch(xor ("GameBase::UpdateTiming_TickratePatch_1"), xor ("GameBase::UpdateTiming"), xor ("DD 05 ?? ?? ?? ?? DC 25 ?? ?? ?? ?? D9 C0 DD 05"), 0x28E, 0x10, MemoryUtilities::IntToByteArray(reinterpret_cast<int>(&tickrate)));
	Memory::AddPatch(xor ("GameBase::UpdateTiming_TickratePatch_2"), xor ("GameBase::UpdateTiming"), xor ("1D ?? ?? ?? ?? DD 05 ?? ?? ?? ?? DC 25"), 0x28E, 0xD, MemoryUtilities::IntToByteArray(reinterpret_cast<int>(&tickrate)));
	Memory::AddPatch(xor ("GameBase::UpdateTiming_TickratePatch_3"), xor ("GameBase::UpdateTiming"), xor ("DD 05 ?? ?? ?? ?? DD 05 ?? ?? ?? ?? DC 25"), 0x28E, 0xE, MemoryUtilities::IntToByteArray(reinterpret_cast<int>(&tickrate)));
	Memory::AddPatch(xor ("GameBase::UpdateTiming_TickratePatch_4"), xor ("GameBase::UpdateTiming"), xor ("DD 1D ?? ?? ?? ?? DD 05 ?? ?? ?? ?? DC 35"), 0x28E, 0xE, MemoryUtilities::IntToByteArray(reinterpret_cast<int>(&tickrate)));

	Memory::AddObject(xor ("GameBase::IsFullscreen"), xor ("55 8B EC 57 56 53 8B F1 80 3D ?? ?? ?? ?? 00 75 05 E9 ?? ?? ?? ?? 81 3D ?? ?? ?? ?? ?? ?? ?? ?? 7C 06 80 7E"), 0xA, 1);
	//taken from osu!rx, it's not actually gamebase::clientbounds, probably a viewport from glcontrol
	Memory::AddObject(xor ("GameBase::ClientBounds"), xor ("89 45 C8 8B 72 0C 8B 15 ?? ?? ?? ?? 83 C2 04 8B 02 89 45 C4 8B 4A 04"), 0x8, 1);

	Memory::AddObject(xor ("GameBase::ClientHash"), xor ("E8 ?? ?? ?? ?? 8B 8D ?? ?? ?? ?? E8 ?? ?? ?? ?? 8D 15 ?? ?? ?? ?? E8 ?? ?? ?? ?? 8B 0D ?? ?? ?? ?? FF 15"), 0x12, 1);
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

	return uniqueIDAddress ? (*reinterpret_cast<CLRString**>(uniqueIDAddress))->Data().data() : L"";
}

uintptr_t GameBase::GetUniqueID2Instance()
{
	const uintptr_t uniqueID2Address = Memory::Objects[xor ("GameBase::UniqueID2")];

	return uniqueID2Address ? *reinterpret_cast<uintptr_t*>(uniqueID2Address) : 0u;
}

std::wstring GameBase::GetUniqueID2()
{
	const uintptr_t uniqueID2Address = Memory::Objects[xor ("GameBase::UniqueID2")];

	return uniqueID2Address ? (*reinterpret_cast<CLRString**>(uniqueID2Address))->Data().data() : L"";
}

uintptr_t GameBase::GetUniqueCheckInstance()
{
	const uintptr_t uniqueCheckAddress = Memory::Objects[xor ("GameBase::UniqueCheck")];

	return uniqueCheckAddress ? *reinterpret_cast<uintptr_t*>(uniqueCheckAddress) : 0u;
}

std::wstring GameBase::GetUniqueCheck()
{
	const uintptr_t uniqueCheckAddress = Memory::Objects[xor ("GameBase::UniqueCheck")];

	return uniqueCheckAddress ? (*reinterpret_cast<CLRString**>(uniqueCheckAddress))->Data().data() : L"";
}
