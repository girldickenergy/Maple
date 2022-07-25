#include "pWebRequest.h"

#include "Vanilla.h"

#include "../Memory.h"
#include "../../Features/Timewarp/Timewarp.h"
#include "../../Config/Config.h"

void __fastcall pWebRequest::addParameterHook(uintptr_t instance, CLRString* name, CLRString* value)
{
    if (name->Data() == L"st" && Config::Timewarp::Enabled)
    {
        const int newValue = static_cast<int>(std::stod(value->Data().data()) * Timewarp::GetRateMultiplier());

        wchar_t buf[16];
        swprintf_s(buf, 16, L"%d", newValue);

        oAddParameter(instance, name, Vanilla::AllocateCLRString(buf));
    }
    else oAddParameter(instance, name, value);
}

void pWebRequest::Initialize()
{
	Memory::AddObject("pWebRequest::AddParameter", "8B 49 24 38 01 FF 74 24 04 6A 01");
	Memory::AddHook("pWebRequest::AddParameter", "pWebRequest::AddParameter", reinterpret_cast<uintptr_t>(addParameterHook), reinterpret_cast<uintptr_t*>(&oAddParameter));
}
