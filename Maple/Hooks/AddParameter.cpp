#include "Hooks.h"

#include "../Config/Config.h"
#include "../Features/Timewarp/Timewarp.h"
#include "../Features/Spoofer/Spoofer.h"
#include "../Sdk/Osu/GameBase.h"

void __fastcall Hooks::AddParameterHook(void* instance, COMString* name, COMString* value)
{
    if (name->Data() == L"st" && Config::Timewarp::Enabled)
    {
        const int newValue = static_cast<int>(std::stod(value->Data().data()) * Timewarp::GetRateMultiplier());

        wchar_t buf[16];
        swprintf_s(buf, 16, L"%d", newValue);

        oAddParameter(instance, name, COMString::CreateString(buf));
    }
    else if (name->Data() == L"c1")
    {
        Spoofer::Update();

        std::wstring uniqueId = GameBase::GetUniqueID() + L"|" + GameBase::GetUniqueID2();
        oAddParameter(instance, name, COMString::CreateString(uniqueId.c_str()));
    }
    else if (name->Data() == L"s")
    {
        Spoofer::Update();
        //todo: figure out if clienthash is being re-generated here
        oAddParameter(instance, name, value);
    }
    else
        oAddParameter(instance, name, value);
}