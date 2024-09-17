#include "VanillaHooking.h"

#include <hde32.h>

VanillaHook* VanillaHooking::findHook(const char* name)
{
	for (const auto& hook : hooks)
		if (hook->Name == name)
			return hook;

	return nullptr;
}

void VanillaHooking::removeHook(const char* name)
{
	for (size_t i = 0; i < hooks.size(); i++)
	{
		if (hooks[i]->Name == name)
		{
			delete hooks[i];
			hooks.erase(hooks.begin() + i);

			break;
		}
	}
}

std::vector<uint8_t> VanillaHooking::getFunctionPrologue(uintptr_t functionAddress, unsigned int minimumBytes)
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

void VanillaHooking::relocateRelativeAddresses(uintptr_t oldLocation, uintptr_t newLocation, unsigned int length)
{
	hde32s hde;
	unsigned int currentLength = 0;
	while (currentLength != length)
	{
		const unsigned int instructionLength = hde32_disasm(reinterpret_cast<void*>(newLocation + currentLength), &hde);
		if (hde.opcode == 0xE9 || hde.opcode == 0xE8) //relative jmp/call
		{
			const intptr_t destinationAddress = static_cast<intptr_t>(oldLocation) + currentLength + instructionLength + *reinterpret_cast<int*>(newLocation + currentLength + 0x1);
			const intptr_t relocatedDestinationAddress = destinationAddress - (static_cast<intptr_t>(newLocation) + currentLength + instructionLength);

			*reinterpret_cast<intptr_t*>(newLocation + currentLength + 0x1) = relocatedDestinationAddress;
		}
		//todo: add support for other jmp and call variants

		currentLength += instructionLength;
	}
}

uintptr_t VanillaHooking::installTrampoline(uintptr_t functionAddress, const std::vector<uint8_t>& functionPrologue)
{
	const uintptr_t trampolineAddress = reinterpret_cast<uintptr_t>(VirtualAlloc(nullptr, functionPrologue.size() + trampolineBytes.size(), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE));

	for (size_t i = 0; i < functionPrologue.size(); i++)
		*reinterpret_cast<uint8_t*>(trampolineAddress + i) = functionPrologue[i];

	relocateRelativeAddresses(functionAddress, trampolineAddress, functionPrologue.size());

	for (unsigned int i = 0; i < trampolineBytes.size(); i++)
		*reinterpret_cast<uint8_t*>(trampolineAddress + functionPrologue.size() + i) = trampolineBytes[i];

	*reinterpret_cast<uintptr_t*>(trampolineAddress + functionPrologue.size() + trampolineAddressOffset) = functionAddress + functionPrologue.size();

	return trampolineAddress;
}

uintptr_t VanillaHooking::installInlineHook(uintptr_t functionAddress, uintptr_t detourAddress, const std::vector<uint8_t>& functionPrologue, bool safe)
{
	const uintptr_t trampolineAddress = installTrampoline(functionAddress, functionPrologue);

	DWORD oldProtect;
	VirtualProtect(reinterpret_cast<LPVOID>(functionAddress), safe ? safeDetourBytes.size() : detourBytes.size(), PAGE_EXECUTE_READWRITE, &oldProtect);

	for (unsigned int i = 0; i < (safe ? safeDetourBytes.size() : detourBytes.size()); i++)
		*reinterpret_cast<uint8_t*>(functionAddress + i) = safe ? safeDetourBytes[i] : detourBytes[i];

	if (safe)
		*reinterpret_cast<intptr_t*>(functionAddress + safeDetourAddressOffset) = static_cast<intptr_t>(detourAddress) - static_cast<intptr_t>(functionAddress) - 0x5;
	else
		*reinterpret_cast<uintptr_t*>(functionAddress + detourAddressOffset) = detourAddress;

	VirtualProtect(reinterpret_cast<LPVOID>(functionAddress), safe ? safeDetourBytes.size() : detourBytes.size(), oldProtect, &oldProtect);

	return trampolineAddress;
}

VanillaResult VanillaHooking::InstallHook(const char* name, uintptr_t functionAddress, uintptr_t detourAddress, uintptr_t* originalFunction, bool safe)
{
	if (findHook(name))
		return VanillaResult::HookAlreadyInstalled;

	const std::vector<uint8_t> functionPrologue = getFunctionPrologue(functionAddress, safe ? safeDetourBytes.size() : detourBytes.size());

	const uintptr_t trampolineAddress = installInlineHook(functionAddress, detourAddress, functionPrologue, safe);

	*originalFunction = trampolineAddress;

	hooks.push_back(new VanillaHook(name, functionAddress, trampolineAddress, functionPrologue));

	return VanillaResult::Success;

	return VanillaResult::UnknownHookType;
}

VanillaResult VanillaHooking::UninstallHook(const char* name)
{
	VanillaHook* hook = findHook(name);
	if (hook)
		return VanillaResult::HookNotInstalled;

	for (size_t i = 0; i < hook->FunctionPrologue.size(); i++)
		*reinterpret_cast<uint8_t*>(hook->FunctionAddress + i) = hook->FunctionPrologue[i];
	
	VirtualFree(reinterpret_cast<LPVOID>(hook->TrampolineAddress), 0, MEM_RELEASE);

	removeHook(name);

	return VanillaResult::Success;

	return VanillaResult::UnknownHookType;
}

void VanillaHooking::UninstallAllHooks()
{
	for (const auto& hook : hooks)
	{
		char hookName[hook->Name.GetSize()];
		hook->Name.GetData(hookName);

		UninstallHook(hookName);
	}
}
