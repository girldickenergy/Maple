#include "StreamingManager.h"

#include "ThemidaSDK.h"

#include "../Memory.h"
#include "../../Config/Config.h"
#include "../../Utilities/Security/xorstr.hpp"
#include "../../Communication/Communication.h"

void __declspec(naked) StreamingManager::pushNewFrameHook(uintptr_t frame)
{
	__asm
	{
		pushad
		pushfd
		cmp [Config::Misc::DisableSpectators], 0x1
		jne orig
		popfd
		popad
		ret
		orig:
		popfd
		popad
		jmp oPushNewFrame
	}
}

void __declspec(naked) StreamingManager::purgeFramesHook(int action, uintptr_t extra)
{
	__asm
	{
		pushad
		pushfd
		cmp [Config::Misc::DisableSpectators], 0x1
		jne orig
		cmp ecx, 0x6
		jne orig
		popfd
		popad
		ret
		orig:
		popfd
		popad
		jmp oPurgeFrames
	}
}

void StreamingManager::Initialize()
{
	VM_FISH_RED_START
	STR_ENCRYPT_START

	Memory::AddObject(xorstr_("StreamingManager::PushNewFrame"), xorstr_("55 8B EC 8B D1 80 3D ?? ?? ?? ?? 00 75 1A A1"));
	Memory::AddObject(xorstr_("StreamingManager::PurgeFrames"), xorstr_("55 8B EC 57 56 53 81 EC ?? ?? ?? ?? 8B F1 8D BD ?? ?? ?? ?? B9 ?? ?? ?? ?? 33 C0 F3 AB 8B CE 89 4D DC 80 3D ?? ?? ?? ?? 00 74 09 80 3D"));

	Memory::AddHook(xorstr_("StreamingManager::PushNewFrame"), xorstr_("StreamingManager::PushNewFrame"), reinterpret_cast<uintptr_t>(pushNewFrameHook), reinterpret_cast<uintptr_t*>(&oPushNewFrame));
	Memory::AddHook(xorstr_("StreamingManager::PurgeFrames"), xorstr_("StreamingManager::PurgeFrames"), reinterpret_cast<uintptr_t>(purgeFramesHook), reinterpret_cast<uintptr_t*>(&oPurgeFrames));

	STR_ENCRYPT_END
	VM_FISH_RED_END
}
