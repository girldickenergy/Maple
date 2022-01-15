#include "Hooks.h"

#include "../Config/Config.h"
#include "../Features/Timewarp/Timewarp.h"

void __fastcall Hooks::AddParameterHook(void* instance, COMString* name, COMString* value)
{
    if (name->Data() == L"st" && Config::Timewarp::Enabled)
    {
        const int newValue = static_cast<int>(std::stod(value->Data().data()) * Timewarp::GetRateMultiplier());

        wchar_t buf[16];
        swprintf_s(buf, 16, L"%d", newValue);

        oAddParameter(instance, name, COMString::CreateString(buf));
    }
    // c1 argument is handled in set_Value hook of Spoofer.
    // s argument is not handled anywhere yet, osu doesn't re-generate client hash.
    else oAddParameter(instance, name, value);
}