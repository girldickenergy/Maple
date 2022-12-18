#pragma once
#include <mutex>
#include <vector>

#include "../../Utilities/Architecture/Singleton.hpp"

class GarbageCollector : public Singleton<GarbageCollector>
{
	static inline std::mutex relocateAddressMutex;

	typedef void(__fastcall* fnRelocateAddress)(uint8_t** block);
	static inline fnRelocateAddress oRelocateAddress;
	static void __stdcall relocateAddressHook(uint8_t** block);
public:
	std::vector<std::reference_wrapper<std::uintptr_t>> Relocations;

	GarbageCollector(singletonLock);

	void Initialize();
};