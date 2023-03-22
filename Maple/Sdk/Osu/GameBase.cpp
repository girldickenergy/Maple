#include "GameBase.h"

#include "VirtualizerSDK.h"

#include "Vanilla.h"
#include "xorstr.hpp"
#include "Utilities/MemoryUtilities.h"
#include "Math/sRectangle.h"

#include "../Memory.h"
#include "../Helpers/Obfuscated.h"
#include "../../Communication/Communication.h"

void GameBase::Initialize()
{
	VIRTUALIZER_FISH_RED_START
	
	Memory::AddObject(xorstr_("GameBase::Time"), xorstr_("80 3D ?? ?? ?? ?? 00 74 1B A1 ?? ?? ?? ?? 2B 05 ?? ?? ?? ?? 3D"), 0xA, 1);
	Memory::AddObject(xorstr_("GameBase::Mode"), xorstr_("80 B8 ?? ?? ?? ?? 00 75 19 A1 ?? ?? ?? ?? 83 F8 0B 74 0B"), 0xA, 1);

	Memory::AddObject(xorstr_("GameBase::UpdateTiming"), xorstr_("8B F1 8B 0D ?? ?? ?? ?? 33 D2 39 09 FF 15 ?? ?? ?? ?? 8B CE FF 15"), 0x16, 2);
	Memory::AddPatch(xorstr_("GameBase::UpdateTiming_TickratePatch_1"), xorstr_("GameBase::UpdateTiming"), xorstr_("DD 05 ?? ?? ?? ?? DC 25 ?? ?? ?? ?? D9 C0 DD 05"), 0x28E, 0x10, MemoryUtilities::IntToByteArray(reinterpret_cast<int>(&tickrate)));
	Memory::AddPatch(xorstr_("GameBase::UpdateTiming_TickratePatch_2"), xorstr_("GameBase::UpdateTiming"), xorstr_("1D ?? ?? ?? ?? DD 05 ?? ?? ?? ?? DC 25"), 0x28E, 0xD, MemoryUtilities::IntToByteArray(reinterpret_cast<int>(&tickrate)));
	Memory::AddPatch(xorstr_("GameBase::UpdateTiming_TickratePatch_3"), xorstr_("GameBase::UpdateTiming"), xorstr_("DD 05 ?? ?? ?? ?? DD 05 ?? ?? ?? ?? DC 25"), 0x28E, 0xE, MemoryUtilities::IntToByteArray(reinterpret_cast<int>(&tickrate)));
	Memory::AddPatch(xorstr_("GameBase::UpdateTiming_TickratePatch_4"), xorstr_("GameBase::UpdateTiming"), xorstr_("DD 1D ?? ?? ?? ?? DD 05 ?? ?? ?? ?? DC 35"), 0x28E, 0xE, MemoryUtilities::IntToByteArray(reinterpret_cast<int>(&tickrate)));

	Memory::AddObject(xorstr_("GameBase::IsFullscreen"), xorstr_("55 8B EC 57 56 53 8B F1 80 3D ?? ?? ?? ?? 00 75 05 E9 ?? ?? ?? ?? 81 3D ?? ?? ?? ?? ?? ?? ?? ?? 7C 06 80 7E"), 0xA, 1);
	Memory::AddObject(xorstr_("GameBase::ClientBounds"), xorstr_("56 FF 75 F0 8B 15 ?? ?? ?? ?? 83 C2 04 39 09"), 0x6, 1);

	Memory::AddObject(xorstr_("GameBase::ClientHash"), xorstr_("EB 0A B9 ?? ?? ?? ?? E8 ?? ?? ?? ?? 50 8B CF BA ?? ?? ?? ?? E8 ?? ?? ?? ?? FF 35 ?? ?? ?? ?? 8B CF BA"), 0x1B, 1);
	Memory::AddObject(xorstr_("GameBase::UniqueID"), xorstr_("8D 4D D0 E8 ?? ?? ?? ?? 8B 35 ?? ?? ?? ?? 8B 0D ?? ?? ?? ?? 8B D0 8B 01"), 0xA, 1);
	Memory::AddObject(xorstr_("GameBase::UniqueID2"), xorstr_("EB 10 E8 ?? ?? ?? ?? 89 85 ?? ?? ?? ?? E8 ?? ?? ?? ?? 8B 1D ?? ?? ?? ?? B9 ?? ?? ?? ?? E8 ?? ?? ?? ?? 8B F8 B9"), 0x14, 1);
	Memory::AddObject(xorstr_("GameBase::UniqueCheck"), xorstr_("C6 40 14 00 8B 1D ?? ?? ?? ?? 8B 0D ?? ?? ?? ?? BA ?? ?? ?? ?? FF 15"), 0x6, 1);

		VIRTUALIZER_FISH_RED_END
}

int GameBase::GetTime()
{
	const uintptr_t timeAddress = Memory::Objects[xorstr_("GameBase::Time")];

	return timeAddress ? *reinterpret_cast<int*>(timeAddress) : 0;
}

OsuModes GameBase::GetMode()
{
	const uintptr_t modeAddress = Memory::Objects[xorstr_("GameBase::Mode")];

	return modeAddress ? *reinterpret_cast<OsuModes*>(modeAddress) : OsuModes::Menu;
}

void GameBase::SetTickrate(double value)
{
	tickrate = value;
}

bool GameBase::GetIsFullscreen()
{
	const uintptr_t isFullscreenAddress = Memory::Objects[xorstr_("GameBase::IsFullscreen")];
	
	return isFullscreenAddress ? *reinterpret_cast<bool*>(isFullscreenAddress) : false;
}

Vector2 GameBase::GetClientSize()
{
	if (const uintptr_t clientBoundsAddress = Memory::Objects[xorstr_("GameBase::ClientBounds")])
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

	if (const uintptr_t clientBoundsAddress = Memory::Objects[xorstr_("GameBase::ClientBounds")])
	{
		const sRectangle* clientBounds = *reinterpret_cast<sRectangle**>(clientBoundsAddress);
		
		return { static_cast<float>(clientBounds->X), static_cast<float>(clientBounds->Y) };
	}

	return { 0.f, 0.f };
}

std::wstring GameBase::GetClientHash()
{
	const uintptr_t clientHashAddress = Memory::Objects[xorstr_("GameBase::ClientHash")];

	return clientHashAddress ? (*reinterpret_cast<CLRString**>(clientHashAddress))->Data().data() : L"";
}

uintptr_t GameBase::GetUniqueIDInstance()
{
	const uintptr_t uniqueIDAddress = Memory::Objects[xorstr_("GameBase::UniqueID")];

	return uniqueIDAddress ? *reinterpret_cast<uintptr_t*>(uniqueIDAddress) : 0u;
}

std::wstring GameBase::GetUniqueID()
{
	const uintptr_t uniqueIDAddress = Memory::Objects[xorstr_("GameBase::UniqueID")];

	return uniqueIDAddress ? Obfuscated::GetString(*reinterpret_cast<uintptr_t*>(uniqueIDAddress))->Data().data() : L"";
}

uintptr_t GameBase::GetUniqueID2Instance()
{
	const uintptr_t uniqueID2Address = Memory::Objects[xorstr_("GameBase::UniqueID2")];

	return uniqueID2Address ? *reinterpret_cast<uintptr_t*>(uniqueID2Address) : 0u;
}

std::wstring GameBase::GetUniqueID2()
{
	const uintptr_t uniqueID2Address = Memory::Objects[xorstr_("GameBase::UniqueID2")];

	return uniqueID2Address ? Obfuscated::GetString(*reinterpret_cast<uintptr_t*>(uniqueID2Address))->Data().data() : L"";
}

uintptr_t GameBase::GetUniqueCheckInstance()
{
	const uintptr_t uniqueCheckAddress = Memory::Objects[xorstr_("GameBase::UniqueCheck")];

	return uniqueCheckAddress ? *reinterpret_cast<uintptr_t*>(uniqueCheckAddress) : 0u;
}

std::wstring GameBase::GetUniqueCheck()
{
	const uintptr_t uniqueCheckAddress = Memory::Objects[xorstr_("GameBase::UniqueCheck")];

	return uniqueCheckAddress ? Obfuscated::GetString(*reinterpret_cast<uintptr_t*>(uniqueCheckAddress))->Data().data() : L"";
}
