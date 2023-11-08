#include "StreamingManager.h"

#include "VirtualizerSDK.h"
#include "xorstr.hpp"

#include "../Memory.h"
#include "../../Configuration/ConfigManager.h"
#include "../../Communication/Communication.h"

void __fastcall StreamingManager::pushNewFrameHook(uintptr_t frame)
{
	if (!ConfigManager::CurrentConfig.Misc.DisableSpectators)
		[[clang::musttail]] return oPushNewFrame(frame);
}

void __fastcall StreamingManager::purgeFramesHook(int action, uintptr_t extra)
{
	if (!ConfigManager::CurrentConfig.Misc.DisableSpectators || action != 6)
		[[clang::musttail]] return oPurgeFrames(action, extra);
}

void StreamingManager::Initialize()
{
	VIRTUALIZER_FISH_RED_START
	
	Memory::AddObject(xorstr_("StreamingManager::PushNewFrame"), xorstr_("55 8B EC 8B D1 80 3D ?? ?? ?? ?? 00 75 1A A1"));
	Memory::AddObject(xorstr_("StreamingManager::PurgeFrames"), xorstr_("55 8B EC 57 56 53 81 EC ?? ?? ?? ?? 8B F1 8D BD ?? ?? ?? ?? B9 ?? ?? ?? ?? 33 C0 F3 AB 8B CE 89 4D DC 80 3D ?? ?? ?? ?? 00 74 09 80 3D"));

	Memory::AddHook(xorstr_("StreamingManager::PushNewFrame"), xorstr_("StreamingManager::PushNewFrame"), reinterpret_cast<uintptr_t>(pushNewFrameHook), reinterpret_cast<uintptr_t*>(&oPushNewFrame));
	Memory::AddHook(xorstr_("StreamingManager::PurgeFrames"), xorstr_("StreamingManager::PurgeFrames"), reinterpret_cast<uintptr_t>(purgeFramesHook), reinterpret_cast<uintptr_t*>(&oPurgeFrames));

	VIRTUALIZER_FISH_RED_END
}
