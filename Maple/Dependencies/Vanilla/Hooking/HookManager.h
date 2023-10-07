#pragma once

#include <string>
#include <memory>
#include <vector>
#include <unordered_map>

#include "Hook.h"
#include "VanillaResult.h"

class HookManager
{
    std::unordered_map<std::string, std::unique_ptr<Hook>> m_Hooks;
    Hook* FindHook(const std::string& name);
    void RemoveHook(const std::string& name);

    static const inline std::vector<uint8_t> DetourBytes = {0x55, 0xBD, 0xFF, 0xFF, 0xFF, 0xFF, 0x87, 0x2C, 0x24, 0xC3};
    static constexpr unsigned int DetourAddressOffset = 2;

    static const inline std::vector<uint8_t> SafeDetourBytes = {0xE9, 0xFF, 0xFF, 0xFF, 0xFF};
    static constexpr unsigned int SafeDetourAddressOffset = 1;

    static const inline std::vector<uint8_t> TrampolineBytes = {0x68, 0xFF, 0xFF, 0xFF, 0xFF, 0xC3};
    static constexpr unsigned int TrampolineAddressOffset = 1;

     std::vector<uint8_t> GetFunctionPrologue(uintptr_t functionAddress, unsigned int minimumBytes);
     void RelocateRelativeAddresses(uintptr_t oldLocation, uintptr_t newLocation, unsigned int length);
     uintptr_t InstallTrampoline(uintptr_t functionAddress, const std::vector<uint8_t>& functionPrologue);
     uintptr_t InstallInlineHook(uintptr_t functionAddress, uintptr_t detourAddress, const std::vector<uint8_t>& functionPrologue, bool safe);

public:
    HookManager() = default;
    ~HookManager();

    VanillaResult InstallHook(const std::string& name, uintptr_t functionAddress, uintptr_t detourAddress, uintptr_t* originalFunction, bool safe = false);
    VanillaResult UninstallHook(const std::string& name);
    void UninstallAllHooks();
};
