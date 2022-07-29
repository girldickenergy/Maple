#include "ObfuscatedString.h"

#include "../Memory.h"
#include "../Osu/GameBase.h"
#include "../../Features/Spoofer/Spoofer.h"

CLRString* __fastcall ObfuscatedString::getValueHook(uintptr_t instance)
{
	if (instance == GameBase::GetUniqueIDInstance())
		return Spoofer::GetUniqueID();

	if (instance == GameBase::GetUniqueID2Instance())
		return Spoofer::GetUniqueID2();

	if (instance == GameBase::GetUniqueCheckInstance())
		return Spoofer::GetUniqueCheck();

	return oGetValue(instance);
}

void __fastcall ObfuscatedString::setValueHook(uintptr_t instance, CLRString* value)
{
	if (instance == GameBase::GetUniqueIDInstance())
		oSetValue(instance, Spoofer::GetUniqueID());
	else if (instance == GameBase::GetUniqueID2Instance())
		oSetValue(instance, Spoofer::GetUniqueID2());
	else if (instance == GameBase::GetUniqueCheckInstance())
		oSetValue(instance, Spoofer::GetUniqueCheck());
	else
		oSetValue(instance, value);
}

void ObfuscatedString::Initialize()
{
	Memory::AddObject("ObfuscatedString::GetValue", "55 8B EC 57 56 53 83 EC 08 8B F1 8B 7E 08 8B 5E 04");
	Memory::AddObject("ObfuscatedString::SetValue", "55 8B EC 57 56 53 83 EC 08 89 55 EC 8B F1 8B 0E");

	Memory::AddHook("ObfuscatedString::GetValue", "ObfuscatedString::GetValue", reinterpret_cast<uintptr_t>(getValueHook), reinterpret_cast<uintptr_t*>(&oGetValue));
	Memory::AddHook("ObfuscatedString::SetValue", "ObfuscatedString::SetValue", reinterpret_cast<uintptr_t>(setValueHook), reinterpret_cast<uintptr_t*>(&oSetValue));
}

CLRString* ObfuscatedString::GetValue(uintptr_t instance)
{
	return oGetValue ? oGetValue(instance) : nullptr;
}

void ObfuscatedString::SetValue(uintptr_t instance, CLRString* value)
{
	if (oSetValue)
		oSetValue(instance, value);
}
