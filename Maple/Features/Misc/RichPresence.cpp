#include "RichPresence.h"

#include "../../Config/Config.h"

void __fastcall RichPresence::SetLargeImageTextHook(void* instance, COMString* string)
{
	if (Config::Misc::RichPresenceSpooferEnabled && string)
	{
		wchar_t buf[128];
		swprintf_s(buf, 128, L"%hs (rank #%hs)", Config::Misc::SpoofedName.c_str(), Config::Misc::SpoofedRank.c_str());

		oSetLargeImageText(instance, COMString::CreateString(buf));
	}
	else
		oSetLargeImageText(instance, string);
}
