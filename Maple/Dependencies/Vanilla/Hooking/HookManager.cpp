#include "HookManager.h"

#include <ranges>

#include "fnv1a.h"
#include "hde32.h"

#include "PointerRedirectionHook.h"
#include "TrampolineHook.h"

VanillaResult HookManager::UninstallHook(uint32_t key)
{
    switch (IHook* hook = m_Hooks[key].get(); hook->GetType())
    {
        case HookTypes::Trampoline:
        {
            TrampolineHook* trampolineHook = dynamic_cast<TrampolineHook*>(hook);

            for (size_t i = 0; i < trampolineHook->GetFunctionPrologue().size(); i++)
                *reinterpret_cast<uint8_t*>(trampolineHook->GetFunctionAddress() + i) = trampolineHook->GetFunctionPrologue()[i];

            VirtualFree(reinterpret_cast<LPVOID>(trampolineHook->GetTrampolineAddress()), 0, MEM_RELEASE);

            break;
        }
        case HookTypes::PointerRedirection:
        {
            PointerRedirectionHook* pointerRedirectionHook = dynamic_cast<PointerRedirectionHook*>(hook);

            *reinterpret_cast<uintptr_t*>(pointerRedirectionHook->GetPointerAddress()) = pointerRedirectionHook->GetOriginalFunctionAddress();

            break;
        }
        default:
            return VanillaResult::HookTypeUnknown;
    }

    m_Hooks.erase(key);

    return VanillaResult::Success;
}

std::vector<uint8_t> HookManager::GetFunctionPrologue(uintptr_t functionAddress, unsigned int minimumBytes)
{
    hde32s hde;

    unsigned int functionPrologueLength = 0;
    while (functionPrologueLength < minimumBytes)
    {
        const unsigned int instructionLength = hde32_disasm(reinterpret_cast<void*>(functionAddress + functionPrologueLength), &hde);

        if (hde.opcode == 0x74 || hde.opcode == 0x75)
            minimumBytes += *reinterpret_cast<uint8_t*>(functionAddress + functionPrologueLength + 1);

        functionPrologueLength += instructionLength;
    }

    std::vector<uint8_t> functionPrologue;
    for (unsigned int i = 0; i < functionPrologueLength; i++)
        functionPrologue.push_back(*reinterpret_cast<uint8_t*>(functionAddress + i));

    return functionPrologue;
}

void HookManager::RelocateRelativeAddresses(uintptr_t oldLocation, uintptr_t newLocation, unsigned int length)
{
    hde32s hde;
    unsigned int currentLength = 0;
    while (currentLength != length)
    {
        const unsigned int instructionLength = hde32_disasm(reinterpret_cast<void*>(newLocation + currentLength), &hde);
        if (hde.opcode == 0xE9) // relative jmp
        {
            const intptr_t destinationAddress = static_cast<intptr_t>(oldLocation) + currentLength + instructionLength + *reinterpret_cast<int*>(newLocation + currentLength + 0x1);
            const intptr_t relocatedDestinationAddress = destinationAddress - (static_cast<intptr_t>(newLocation) + currentLength + instructionLength);

            *reinterpret_cast<intptr_t*>(newLocation + currentLength + 0x1) = relocatedDestinationAddress;
        }
        // todo: add support for call instruction and other jmp variants

        currentLength += instructionLength;
    }
}

uintptr_t HookManager::InstallTrampoline(uintptr_t functionAddress, const std::vector<uint8_t>& functionPrologue)
{
    const uintptr_t trampolineAddress = reinterpret_cast<uintptr_t>(VirtualAlloc(nullptr, functionPrologue.size() + TrampolineBytes.size(), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE));

    for (size_t i = 0; i < functionPrologue.size(); i++)
        *reinterpret_cast<uint8_t*>(trampolineAddress + i) = functionPrologue[i];

    RelocateRelativeAddresses(functionAddress, trampolineAddress, functionPrologue.size());

    for (unsigned int i = 0; i < TrampolineBytes.size(); i++)
        *reinterpret_cast<uint8_t*>(trampolineAddress + functionPrologue.size() + i) = TrampolineBytes[i];

    *reinterpret_cast<uintptr_t*>(trampolineAddress + functionPrologue.size() + TrampolineAddressOffset) = functionAddress + functionPrologue.size();

    return trampolineAddress;
}

uintptr_t HookManager::InstallInlineHook(uintptr_t functionAddress, uintptr_t detourAddress, const std::vector<uint8_t>& functionPrologue, bool stealthy)
{
    const uintptr_t trampolineAddress = InstallTrampoline(functionAddress, functionPrologue);

    DWORD oldProtect;
    VirtualProtect(reinterpret_cast<LPVOID>(functionAddress), stealthy ? StealthyDetourBytes.size() : DetourBytes.size(), PAGE_EXECUTE_READWRITE, &oldProtect);

    for (unsigned int i = 0; i < (stealthy ? StealthyDetourBytes.size() : DetourBytes.size()); i++)
        *reinterpret_cast<uint8_t*>(functionAddress + i) = stealthy ? StealthyDetourBytes[i] : DetourBytes[i];

    if (stealthy)
        *reinterpret_cast<intptr_t*>(functionAddress + StealthyDetourAddressOffset) = detourAddress;
    else
        *reinterpret_cast<uintptr_t*>(functionAddress + DetourAddressOffset) = static_cast<intptr_t>(detourAddress) - static_cast<intptr_t>(functionAddress) - 0x5;

    VirtualProtect(reinterpret_cast<LPVOID>(functionAddress), stealthy ? StealthyDetourBytes.size() : DetourBytes.size(), oldProtect, &oldProtect);

    return trampolineAddress;
}

HookManager::~HookManager()
{
    UninstallAllHooks();
}

VanillaResult HookManager::InstallTrampolineHook(const char* name, uintptr_t functionAddress, uintptr_t detourAddress, uintptr_t* originalFunction, bool stealthy)
{
    if (m_Hooks.contains(fnv1a::Hash(name)))
        return VanillaResult::HookAlreadyInstalled;

    const std::vector<uint8_t> functionPrologue = GetFunctionPrologue(functionAddress, stealthy ? StealthyDetourBytes.size() : DetourBytes.size());

    const uintptr_t trampolineAddress = InstallInlineHook(functionAddress, detourAddress, functionPrologue, stealthy);

    *originalFunction = trampolineAddress;

    m_Hooks[fnv1a::Hash(name)] = std::make_unique<TrampolineHook>(functionAddress, trampolineAddress, functionPrologue);

    return VanillaResult::Success;
}

VanillaResult HookManager::InstallPointerRedirectionHook(const char* name, uintptr_t pointerAddress, uintptr_t detourAddress, uintptr_t* originalFunction)
{
    if (m_Hooks.contains(fnv1a::Hash(name)))
        return VanillaResult::HookAlreadyInstalled;

    *originalFunction = *reinterpret_cast<uintptr_t*>(pointerAddress);

    *reinterpret_cast<uintptr_t*>(pointerAddress) = detourAddress;

    m_Hooks[fnv1a::Hash(name)] = std::make_unique<PointerRedirectionHook>(pointerAddress, *originalFunction);

    return VanillaResult::Success;
}

VanillaResult HookManager::UninstallHook(const char* name)
{
    if (!m_Hooks.contains(fnv1a::Hash(name)))
        return VanillaResult::HookNotInstalled;

    return UninstallHook(fnv1a::Hash(name));
}

void HookManager::UninstallAllHooks()
{
    for (const uint32_t key : m_Hooks | std::views::keys)
        UninstallHook(key);
}
