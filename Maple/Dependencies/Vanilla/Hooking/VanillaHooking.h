#pragma once

#include <string>
#include <vector>

#include "VanillaHook.h"
#include "VanillaResult.h"

class VanillaHooking
{
	static inline std::vector<VanillaHook*> hooks;
	static VanillaHook* findHook(const std::string& name);
	static void removeHook(const std::string& name);

	static inline std::vector<uint8_t> detourBytes = { 0x55, 0xBD, 0xFF, 0xFF, 0xFF, 0xFF, 0x83, 0xEC, 0x04, 0x87, 0x2C, 0x24, 0xC3 };
	static inline constexpr unsigned int detourAddressOffset = 2;

	static inline std::vector<uint8_t> detourStubBytes = { 0x5D, 0x68, 0xFF, 0xFF, 0xFF, 0xFF, 0xC3 };
	static inline constexpr unsigned int detourStubAddressOffset = 2;

	static inline std::vector<uint8_t> trampolineBytes = { 0x68, 0xFF, 0xFF, 0xFF, 0xFF, 0xC3 };
	static inline constexpr unsigned int trampolineAddressOffset = 1;

	static std::vector<uint8_t> getFunctionPrologue(uintptr_t functionAddress, unsigned int minimumBytes);
	static void relocateRelativeAddresses(uintptr_t oldLocation, uintptr_t newLocation, unsigned int length);
	static uintptr_t installDetourStub(uintptr_t detourAddress);
	static uintptr_t installTrampoline(uintptr_t functionAddress, uintptr_t detourAddress, const std::vector<uint8_t>& functionPrologue);
	static uintptr_t installInlineHook(uintptr_t functionAddress, uintptr_t detourStubAddress, uintptr_t detourAddress, const std::vector<uint8_t>& functionPrologue);
public:
	static VanillaResult InstallHook(const std::string& name, uintptr_t functionAddress, uintptr_t detourAddress, uintptr_t* originalFunction);
	static VanillaResult UninstallHook(const std::string& name);
	static void UninstallAllHooks();
};
