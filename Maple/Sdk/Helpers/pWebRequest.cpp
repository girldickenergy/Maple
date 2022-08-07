#include "pWebRequest.h"

#include "ThemidaSDK.h"
#include "Vanilla.h"

#include "../Memory.h"
#include "../../Features/Timewarp/Timewarp.h"
#include "../../Config/Config.h"
#include "../../Utilities/Security/xorstr.hpp"

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
    STR_ENCRYPT_START

	Memory::AddObject(xor ("pWebRequest::AddParameter"), xor ("8B 49 24 38 01 FF 74 24 04 6A 01"));
	Memory::AddHook(xor ("pWebRequest::AddParameter"), xor ("pWebRequest::AddParameter"), reinterpret_cast<uintptr_t>(addParameterHook), reinterpret_cast<uintptr_t*>(&oAddParameter));

    STR_ENCRYPT_END
}
