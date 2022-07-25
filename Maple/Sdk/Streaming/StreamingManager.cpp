#include "StreamingManager.h"

#include "../Memory.h"
#include "../../Config/Config.h"

void __fastcall StreamingManager::pushNewFrameHook(uintptr_t frame)
{
	if (!Config::Misc::DisableSpectators)
		oPushNewFrame(frame);
}

void __declspec(naked) StreamingManager::purgeFramesHook(int action, uintptr_t extra)
{
	__asm
	{
		cmp[Config::Misc::DisableSpectators], 0x1
		jne orig
		cmp ecx, 0x6
		jne orig
		ret
		orig :
		jmp oPurgeFrames
	}
}

void StreamingManager::Initialize()
{
	Memory::AddObject("StreamingManager::PushNewFrame", "55 8B EC 8B D1 80 3D ?? ?? ?? ?? 00 75 1A A1");
	Memory::AddObject("StreamingManager::PurgeFrames", "55 8B EC 57 56 53 81 EC ?? ?? ?? ?? 8B F1 8D BD ?? ?? ?? ?? B9 ?? ?? ?? ?? 33 C0 F3 AB 8B CE 89 4D DC 80 3D ?? ?? ?? ?? 00 74 09 80 3D");

	Memory::AddHook("StreamingManager::PushNewFrame", "StreamingManager::PushNewFrame", reinterpret_cast<uintptr_t>(pushNewFrameHook), reinterpret_cast<uintptr_t*>(&oPushNewFrame));
	Memory::AddHook("StreamingManager::PurgeFrames", "StreamingManager::PurgeFrames", reinterpret_cast<uintptr_t>(purgeFramesHook), reinterpret_cast<uintptr_t*>(&oPurgeFrames));
}
