#include "Obfuscated.h"

#include "ThemidaSDK.h"

#include "../Memory.h"
#include "../Osu/GameBase.h"
#include "../../Features/Spoofer/Spoofer.h"
#include "../../Utilities/Security/xorstr.hpp"
#include "../../Communication/Communication.h"

CLRString* __fastcall Obfuscated::getStringValueHook(uintptr_t instance)
{
	if (instance == GameBase::GetUniqueIDInstance())
		return Spoofer::GetUniqueID();

	if (instance == GameBase::GetUniqueID2Instance())
		return Spoofer::GetUniqueID2();

	if (instance == GameBase::GetUniqueCheckInstance())
		return Spoofer::GetUniqueCheck();

	return oGetStringValue(instance);
}

void __fastcall Obfuscated::setStringValueHook(uintptr_t instance, CLRString* value)
{
	if (instance == GameBase::GetUniqueIDInstance())
		oSetStringValue(instance, Spoofer::GetUniqueID());
	else if (instance == GameBase::GetUniqueID2Instance())
		oSetStringValue(instance, Spoofer::GetUniqueID2());
	else if (instance == GameBase::GetUniqueCheckInstance())
		oSetStringValue(instance, Spoofer::GetUniqueCheck());
	else
		oSetStringValue(instance, value);
}

void Obfuscated::Initialize()
{
	VM_FISH_RED_START
	STR_ENCRYPT_START

	Memory::AddObject(xorstr_("ObfuscatedString::GetValue"), xorstr_("55 8B EC 57 56 53 83 EC 08 8B F1 8B 7E 08 8B 5E 04 8B 46 0C 89 45 F0 8B 0E 8B 41 20 8B 00 8B 40 0C 85 C0 75 0A"));
	Memory::AddObject(xorstr_("ObfuscatedString::SetValue"), xorstr_("55 8B EC 57 56 53 83 EC 08 89 55 EC 8B F1 8B 0E E8 ?? ?? ?? ?? 8B 08 39 09 FF 15 ?? ?? ?? ?? 89 46 0C 8B 7E 08 8B 46 0C 89 45 F0 8B 0E"));

	Memory::AddHook(xorstr_("ObfuscatedString::GetValue"), xorstr_("ObfuscatedString::GetValue"), reinterpret_cast<uintptr_t>(getStringValueHook), reinterpret_cast<uintptr_t*>(&oGetStringValue));
	Memory::AddHook(xorstr_("ObfuscatedString::SetValue"), xorstr_("ObfuscatedString::SetValue"), reinterpret_cast<uintptr_t>(setStringValueHook), reinterpret_cast<uintptr_t*>(&oSetStringValue));

	STR_ENCRYPT_END
	VM_FISH_RED_END
}

CLRString* Obfuscated::GetString(uintptr_t instance)
{
	return oGetStringValue ? oGetStringValue(instance) : nullptr;
}

void Obfuscated::SetString(uintptr_t instance, CLRString* value)
{
	if (oSetStringValue)
		oSetStringValue(instance, value);
}

int Obfuscated::GetInt(uintptr_t instance)
{
	if (instance)
	{
		const int value = *reinterpret_cast<int*>(instance + VALUE_OFFSET);
		const int key = *reinterpret_cast<int*>(instance + KEY_OFFSET);

		return value ^ key;
	}

	return 0;
}

void Obfuscated::SetInt(uintptr_t instance, int value)
{
	if (instance)
	{
		const int key = *reinterpret_cast<int*>(instance + KEY_OFFSET);

		*reinterpret_cast<int*>(instance + VALUE_OFFSET) = value ^ key;
	}
}
