#include "RichPresence.h"

#include "../../Config/Config.h"

void __fastcall RichPresence::SetLargeImageTextHook(void* instance, COMString* string)
{
	if (Config::Misc::RichPresenceSpooferEnabled && string)
	{
		spoofedString->vtable = *reinterpret_cast<void**>(string);
		swprintf_s(spoofedString->buffer, 128, L"%hs (rank #%hs)", Config::Misc::SpoofedName.c_str(), Config::Misc::SpoofedRank.c_str());
		spoofedString->length = wcslen(spoofedString->buffer);

		oSetLargeImageText(instance, spoofedString);
	}
	else
		oSetLargeImageText(instance, string);
}
