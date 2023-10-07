#include "HookManager.h"

#include <ranges>

#include "hde32.h"

Hook* HookManager::FindHook(const std::string& name)
{
    if (m_Hooks.contains(name))
        return m_Hooks[name].get();

    return nullptr;
}

void HookManager::RemoveHook(const std::string& name)
{
    if (m_Hooks.contains(name))
        m_Hooks.erase(name);
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

uintptr_t HookManager::InstallInlineHook(uintptr_t functionAddress, uintptr_t detourAddress, const std::vector<uint8_t>& functionPrologue, bool safe)
{
    const uintptr_t trampolineAddress = InstallTrampoline(functionAddress, functionPrologue);

    DWORD oldProtect;
    VirtualProtect(reinterpret_cast<LPVOID>(functionAddress), safe ? SafeDetourBytes.size() : DetourBytes.size(), PAGE_EXECUTE_READWRITE, &oldProtect);

    for (unsigned int i = 0; i < (safe ? SafeDetourBytes.size() : DetourBytes.size()); i++)
        *reinterpret_cast<uint8_t*>(functionAddress + i) = safe ? SafeDetourBytes[i] : DetourBytes[i];

    if (safe)
        *reinterpret_cast<intptr_t*>(functionAddress + SafeDetourAddressOffset) = static_cast<intptr_t>(detourAddress) - static_cast<intptr_t>(functionAddress) - 0x5;
    else
        *reinterpret_cast<uintptr_t*>(functionAddress + DetourAddressOffset) = detourAddress;

    VirtualProtect(reinterpret_cast<LPVOID>(functionAddress), safe ? SafeDetourBytes.size() : DetourBytes.size(), oldProtect, &oldProtect);

    return trampolineAddress;
}

HookManager::~HookManager()
{
    UninstallAllHooks();
}

VanillaResult HookManager::InstallHook(const std::string& name, uintptr_t functionAddress, uintptr_t detourAddress, uintptr_t* originalFunction, bool safe)
{
    if (FindHook(name))
        return VanillaResult::HookAlreadyInstalled;

    const std::vector<uint8_t> functionPrologue = GetFunctionPrologue(functionAddress, safe ? SafeDetourBytes.size() : DetourBytes.size());

    const uintptr_t trampolineAddress = InstallInlineHook(functionAddress, detourAddress, functionPrologue, safe);

    *originalFunction = trampolineAddress;

    m_Hooks[name] = std::make_unique<Hook>(functionAddress, trampolineAddress, functionPrologue);

    return VanillaResult::Success;
}

VanillaResult HookManager::UninstallHook(const std::string& name)
{
    const Hook* hook = FindHook(name);
    if (!hook)
        return VanillaResult::HookNotInstalled;

    for (size_t i = 0; i < hook->FunctionPrologue.size(); i++)
        *reinterpret_cast<uint8_t*>(hook->FunctionAddress + i) = hook->FunctionPrologue[i];

    VirtualFree(reinterpret_cast<LPVOID>(hook->TrampolineAddress), 0, MEM_RELEASE);

    RemoveHook(name);

    return VanillaResult::Success;
}

void HookManager::UninstallAllHooks()
{
    for (const std::string& key : m_Hooks | std::views::keys)
        UninstallHook(key);
}
