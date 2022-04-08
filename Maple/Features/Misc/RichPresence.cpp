#include "RichPresence.h"

#include "../../Config/Config.h"

void __fastcall RichPresence::SetLargeImageTextHook(void* instance, COMString* string)
{
	if (Config::Misc::RichPresenceSpoofer::Enabled && string)
	{
		wchar_t buf[128];
		swprintf_s(buf, 128, L"%hs (rank #%hs)", Config::Misc::RichPresenceSpoofer::Name, Config::Misc::RichPresenceSpoofer::Rank);

		oSetLargeImageText(instance, COMString::CreateString(buf));
	}
	else
		oSetLargeImageText(instance, string);
}
