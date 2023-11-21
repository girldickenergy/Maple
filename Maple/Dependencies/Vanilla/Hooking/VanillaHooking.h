#pragma once

#include <string>
#include <vector>

#include "VanillaHook.h"
#include "VanillaResult.h"

class VanillaHooking
{
	static inline std::vector<VanillaHook*> hooks;
	static VanillaHook* findHook(const char* name);
	static void removeHook(const char* name);

	static inline std::vector<uint8_t> detourBytes = { 0x55, 0xBD, 0xFF, 0xFF, 0xFF, 0xFF, 0x87, 0x2C, 0x24, 0xC3 };
	static inline constexpr unsigned int detourAddressOffset = 2;
	
	static inline std::vector<uint8_t> safeDetourBytes = { 0xE9, 0xFF, 0xFF, 0xFF, 0xFF };
	static inline constexpr unsigned int safeDetourAddressOffset = 1;

	static inline std::vector<uint8_t> trampolineBytes = { 0x68, 0xFF, 0xFF, 0xFF, 0xFF, 0xC3 };
	static inline constexpr unsigned int trampolineAddressOffset = 1;

	static std::vector<uint8_t> getFunctionPrologue(uintptr_t functionAddress, unsigned int minimumBytes);
	static void relocateRelativeAddresses(uintptr_t oldLocation, uintptr_t newLocation, unsigned int length);
	static uintptr_t installTrampoline(uintptr_t functionAddress, const std::vector<uint8_t>& functionPrologue);
	static uintptr_t installInlineHook(uintptr_t functionAddress, uintptr_t detourAddress, const std::vector<uint8_t>& functionPrologue, bool safe);
public:
	static VanillaResult InstallHook(const char* name, uintptr_t functionAddress, uintptr_t detourAddress, uintptr_t* originalFunction, bool safe = false);
	static VanillaResult UninstallHook(const char* name);
	static void UninstallAllHooks();
};
