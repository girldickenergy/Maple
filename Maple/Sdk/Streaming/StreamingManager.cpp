#include "StreamingManager.h"

#include "ThemidaSDK.h"

#include "../Memory.h"
#include "../../Config/Config.h"
#include "../../Utilities/Security/xorstr.hpp"

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
	STR_ENCRYPT_START

	Memory::AddObject(xor ("StreamingManager::PushNewFrame"), xor ("55 8B EC 8B D1 80 3D ?? ?? ?? ?? 00 75 1A A1"));
	Memory::AddObject(xor ("StreamingManager::PurgeFrames"), xor ("55 8B EC 57 56 53 81 EC ?? ?? ?? ?? 8B F1 8D BD ?? ?? ?? ?? B9 ?? ?? ?? ?? 33 C0 F3 AB 8B CE 89 4D DC 80 3D ?? ?? ?? ?? 00 74 09 80 3D"));

	Memory::AddHook(xor ("StreamingManager::PushNewFrame"), xor ("StreamingManager::PushNewFrame"), reinterpret_cast<uintptr_t>(pushNewFrameHook), reinterpret_cast<uintptr_t*>(&oPushNewFrame));
	Memory::AddHook(xor ("StreamingManager::PurgeFrames"), xor ("StreamingManager::PurgeFrames"), reinterpret_cast<uintptr_t>(purgeFramesHook), reinterpret_cast<uintptr_t*>(&oPurgeFrames), VanillaHookType::UndetectedInline);

	STR_ENCRYPT_END
}
