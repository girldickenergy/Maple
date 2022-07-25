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
