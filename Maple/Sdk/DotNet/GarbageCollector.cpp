#include "GarbageCollector.h"

#include "../Memory.h"
#include "ThemidaSDK.h"
#include "../../Utilities/Security/xorstr.hpp"

void GarbageCollector::relocateAddressHook(uint8_t** block)
{
	if (*block != nullptr)
	{
		std::unique_lock lock(relocateAddressMutex);

		for (auto& relocation : Get().Relocations)
		{
			if (relocation != reinterpret_cast<uintptr_t>(*block)) continue;

			oRelocateAddress(block);

			relocation.get() = reinterpret_cast<uintptr_t>(*block);
			return;
		}
	}
	oRelocateAddress(block);
}

GarbageCollector::GarbageCollector(singletonLock) { }

void GarbageCollector::Initialize()
{
	VM_FISH_RED_START
	STR_ENCRYPT_START
	Memory::AddObject(xorstr_("GarbageCollector::RelocateAddress"), xorstr_("55 8B EC 57 8B 7D 08 8B 0F 3B 0D"));

	Memory::AddHook(xorstr_("GarbageCollector::RelocateAddress"), xorstr_("GarbageCollector::RelocateAddress"),
		reinterpret_cast<uintptr_t>(relocateAddressHook), reinterpret_cast<uintptr_t*>(&oRelocateAddress));
	STR_ENCRYPT_END
	VM_FISH_RED_END
}