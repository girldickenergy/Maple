#include "SpectateHandler.h"

#include "../../Config/Config.h"

__declspec(naked) void SpectateHandler::PushNewFrameHook(void* bReplayFrame_f)
{
	__asm
	{
		cmp [Config::Misc::DisableSpectators], 0x1
		jne orig
		ret
		orig:
		jmp oPushNewFrame
	}
}

__declspec(naked) void SpectateHandler::PurgeFramesHook(int action, int extra)
{
	__asm
	{
		cmp [Config::Misc::DisableSpectators], 0x1
		jne orig
		cmp ecx, 0x6
		jne orig
		ret
		orig:
		jmp oPurgeFrames
	}
}
