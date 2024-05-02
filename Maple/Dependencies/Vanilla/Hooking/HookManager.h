#pragma once

#include <memory>
#include <vector>
#include <unordered_map>

#include "IHook.h"
#include "VanillaResult.h"

class HookManager
{
    static const inline std::vector<uint8_t> DetourBytes = {0xE9, 0xFF, 0xFF, 0xFF, 0xFF};
    static constexpr unsigned int DetourAddressOffset = 1;

    static const inline std::vector<uint8_t> StealthyDetourBytes = {0x55, 0xBD, 0xFF, 0xFF, 0xFF, 0xFF, 0x87, 0x2C, 0x24, 0xC3};
    static constexpr unsigned int StealthyDetourAddressOffset = 2;

    static const inline std::vector<uint8_t> TrampolineBytes = {0x68, 0xFF, 0xFF, 0xFF, 0xFF, 0xC3};
    static constexpr unsigned int TrampolineAddressOffset = 1;

    std::unordered_map<uint32_t, std::unique_ptr<IHook>> m_Hooks;

    VanillaResult UninstallHook(uint32_t key);
    std::vector<uint8_t> GetFunctionPrologue(uintptr_t functionAddress, unsigned int minimumBytes);
    void RelocateRelativeAddresses(uintptr_t oldLocation, uintptr_t newLocation, unsigned int length);
    uintptr_t InstallTrampoline(uintptr_t functionAddress, const std::vector<uint8_t>& functionPrologue);
    uintptr_t InstallInlineHook(uintptr_t functionAddress, uintptr_t detourAddress, const std::vector<uint8_t>& functionPrologue, bool stealthy);

public:
    HookManager() = default;
    ~HookManager();

    VanillaResult InstallTrampolineHook(const char* name, uintptr_t functionAddress, uintptr_t detourAddress, uintptr_t* originalFunction, bool stealthy = true);
    VanillaResult InstallPointerRedirectionHook(const char* name, uintptr_t pointerAddress, uintptr_t detourAddress, uintptr_t* originalFunction);
    VanillaResult UninstallHook(const char* name);
    void UninstallAllHooks();
};
