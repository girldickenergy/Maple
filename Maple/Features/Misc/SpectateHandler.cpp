#include "SpectateHandler.h"

#include "../../Config/Config.h"

void __fastcall SpectateHandler::PushNewFrameHook(void* bReplayFrame_f)
{
	if (!Config::Misc::DisableSpectators)
		oPushNewFrame(bReplayFrame_f);
}

void __fastcall SpectateHandler::PurgeFramesHook(int action, void* extra)
{
	if (!Config::Misc::DisableSpectators || action != 0x6)
		oPurgeFrames(action, extra);
}
