#include "Vanilla.h"

#include "Hooking/VanillaHooking.h"
#include "Patching/VanillaPatcher.h"
#include "PatternScanning/VanillaPatternScanner.h"
#include "Utilities/MemoryUtilities.h"

int __stdcall Vanilla::compileMethodHook(uintptr_t thisvar, uintptr_t compHnd, uintptr_t methodInfo, unsigned int flags, uintptr_t* entryAddress, unsigned int* nativeSizeOfCode)
{
	uintptr_t nativeCodeAddress = 0u;

	const int ret = oCompileMethod(thisvar, compHnd, methodInfo, flags, &nativeCodeAddress, nativeSizeOfCode);

	if (jitCallback)
		jitCallback(nativeCodeAddress, *nativeSizeOfCode);

	*entryAddress = nativeCodeAddress;

	return ret;
}

void __stdcall Vanilla::relocateAddressHook(uint8_t** block)
{
	if (*block != nullptr)
	{
		std::unique_lock lock(mutex);

		for (auto& relocation : relocations)
		{
			if (relocation == reinterpret_cast<uintptr_t>(*block))
			{
				oRelocateAddress(block);

				relocation.get() = reinterpret_cast<uintptr_t>(*block);
				return;
			}
		}
	}

	oRelocateAddress(block);
}

VanillaResult Vanilla::Initialize(bool useCLR)
{
	usingCLR = useCLR;
	
	if (usingCLR)
	{
		void* compileMethodAddress = reinterpret_cast<void*>(VanillaPatternScanner::FindPatternInModule("55 8B EC 83 E4 F8 83 EC 1C 53 8B 5D 10", "clrjit.dll"));
		if (!compileMethodAddress)
			return VanillaResult::JITFailure;
		
		if (VanillaHooking::InstallHook("JITHook", reinterpret_cast<uintptr_t>(compileMethodAddress), reinterpret_cast<uintptr_t>(compileMethodHook), reinterpret_cast<uintptr_t*>(&oCompileMethod)) != VanillaResult::Success)
			return VanillaResult::JITFailure;

		void* relocateAddressAddress = reinterpret_cast<void*>(VanillaPatternScanner::FindPatternInModule("55 8B EC 57 8B 7D 08 8B 0F 3B 0D", "clr.dll"));
		if (!relocateAddressAddress)
			return VanillaResult::RelocateFailure;

		if (VanillaHooking::InstallHook("RelocateAddressHook", reinterpret_cast<uintptr_t>(relocateAddressAddress), reinterpret_cast<uintptr_t>(relocateAddressHook), reinterpret_cast<uintptr_t*>(&oRelocateAddress)) != VanillaResult::Success)
			return VanillaResult::RelocateFailure;

		void* allocateCLRStringAddress = reinterpret_cast<void*>(VanillaPatternScanner::FindPatternInModule("53 8B D9 56 57 85 DB 0F", "clr.dll"));
		if (!allocateCLRStringAddress)
			return VanillaResult::CLRStringFailure;

		allocateCLRString = static_cast<fnAllocateCLRString>(allocateCLRStringAddress);
	}

	return VanillaResult::Success;
}

void Vanilla::Shutdown()
{
	VanillaPatcher::UninstallAllPatches();
	VanillaHooking::UninstallAllHooks();

	if (usingCLR)
		jitCallback = nullptr;
}

void Vanilla::SetJITCallback(void* callback)
{
	if (usingCLR)
		jitCallback = static_cast<fnJITCallback>(callback);
}

void Vanilla::RemoveJITCallback()
{
	if (usingCLR)
		jitCallback = nullptr;
}

void Vanilla::AddRelocation(std::reference_wrapper<std::uintptr_t> relocation)
{
	if (usingCLR)
		relocations.push_back(relocation);
}

void Vanilla::RemoveRelocation(std::reference_wrapper<std::uintptr_t> relocation)
{
	if (!usingCLR)
		return;
	
	for (auto it = relocations.begin(); it != relocations.end(); ++it)
	{
		if (it->get() == relocation.get())
		{
			relocations.erase(it);

			return;
		}
	}
}

CLRString* Vanilla::AllocateCLRString(const wchar_t* pwsz)
{
	if (usingCLR)
		return allocateCLRString(pwsz);

	return nullptr;
}

bool Vanilla::CheckAddressInModule(uintptr_t address, const std::string& moduleName)
{
	const HMODULE module = GetModuleHandleA(moduleName.c_str());
	const uintptr_t moduleBase = reinterpret_cast<uintptr_t>(module);
	const unsigned long moduleSize = MemoryUtilities::GetModuleSize(module);

	return address >= moduleBase && address < moduleBase + moduleSize;
}
