#include "Milk.h"

#include <intrin.h>

#include "../Vanilla/PatternScanning/VanillaPatternScanner.h"
#include "../../Utilities/Security/xorstr.hpp"
#include "../../Logging/Logger.h"
#include "../../Utilities/Security/Security.h"
#include <ThemidaSDK.h>
#include <Hooking/VanillaHooking.h>

#pragma optimize("", off)

Milk::Milk(singletonLock)
{
	VM_FISH_RED_START
	_milkMemory = MilkMemory();
	_authStubBaseAddress = 0x00000000;
	_firstCRCAddress = 0x00000000;
	_firstCRC = nullptr;
	preparationSuccess = false;

	if (prepare())
		preparationSuccess = true;

	VM_FISH_RED_END
}

Milk::~Milk()
{
	VM_FISH_RED_START
	_milkMemory.~MilkMemory();
	VM_FISH_RED_END
}

uintptr_t __stdcall Milk::getJitHook()
{
	const uint32_t STUB_SIZE = 0x7F5000;
	const uint32_t BUFFER = 0x1000;

	auto retAddress = reinterpret_cast<uintptr_t>(_ReturnAddress());
	bool isAuthCall = retAddress > Get()._authStubBaseAddress && retAddress < Get()._authStubBaseAddress + STUB_SIZE +
		BUFFER;

	if (isAuthCall)
		return reinterpret_cast<uintptr_t>(&_realJITVtable);

	return oGetJit();
}

uintptr_t Milk::findAuthStub()
{
	VM_LION_BLACK_START
	for (const auto& region : *_milkMemory.GetMemoryRegions())
		if (region.State != MEM_FREE && region.Protect == PAGE_EXECUTE)
			return region.BaseAddress;

	return 0;
	VM_LION_BLACK_END
}

// ReSharper disable once CppInconsistentNaming
uintptr_t Milk::findFirstCRCAddress()
{
	VM_LION_BLACK_START
	STR_ENCRYPT_START
	auto pattern = xorstr_("5D C3 CC 55 8B EC B9 ?? ?? ?? ?? E8 ?? ?? ?? ?? 5D C3 CC 55 8B EC");
	STR_ENCRYPT_END
	for (const auto& region : *_milkMemory.GetMemoryRegions())
	{
		if (region.BaseAddress < _authStubBaseAddress)
			continue;

		uintptr_t result = VanillaPatternScanner::FindPatternInRange(pattern, region.BaseAddress, region.RegionSize, 7);

		if (result > _authStubBaseAddress)
			return result;
	}

	return 0;
	VM_LION_BLACK_END
}

bool Milk::doCRCBypass()
{
	VM_LION_BLACK_START
	if (_firstCRC == nullptr)
		return false;

	_firstCRC->nextEntry = nullptr;
	if (_firstCRC->nextEntry != nullptr)
		return false;

	return true;
	VM_LION_BLACK_END
}

bool Milk::DoBypass()
{
	VM_LION_BLACK_START
	STR_ENCRYPT_START

	if (!preparationSuccess)
		return false;

	if (!doCRCBypass())
		return false;

	Logger::Log(LogSeverity::Debug, xorstr_("[Milk] Success!"));

	return true;
	STR_ENCRYPT_END
	VM_LION_BLACK_END
}

void Milk::HookJITVtable(int index, uintptr_t detour, uintptr_t* originalFunction)
{
	*originalFunction = _fakeJITVtable[index];
	_fakeJITVtable[index] = detour;
}

bool Milk::prepare()
{
	VM_LION_BLACK_START
	STR_ENCRYPT_START

	_authStubBaseAddress = findAuthStub();
	if (_authStubBaseAddress == 0x00000000)
		return false;
	Logger::Log(LogSeverity::Debug, xorstr_("[Milk] AS != 0x00000000"));

	_firstCRCAddress = findFirstCRCAddress();
	if (_firstCRCAddress == 0x00000000)
		return false;
	Logger::Log(LogSeverity::Debug, xorstr_("[Milk] FC != 0x00000000"));

	_firstCRC = **reinterpret_cast<CRC***>(_firstCRCAddress);
	Logger::Log(LogSeverity::Debug, _firstCRC->className);
	Logger::Log(LogSeverity::Debug, _firstCRC->functionName);
	Logger::Log(LogSeverity::Debug, std::to_string(_firstCRC->functionSize).c_str());
	if (_firstCRC->functionSize < 5 || _firstCRC->functionSize > 1000)
		return false;

	void* getJit = GetProcAddress(GetModuleHandleA(xorstr_("clrjit.dll")), xorstr_("getJit"));
	if (!getJit)
		return false;

	uintptr_t jit = static_cast<fnGetJit>(getJit)();
	if (!jit)
		return false;

	_realJITVtable = *reinterpret_cast<uintptr_t*>(jit);
	if (!_realJITVtable)
		return false;

	_fakeJITVtable = new uintptr_t[7];
	memcpy(_fakeJITVtable, reinterpret_cast<void*>(_realJITVtable), 7 * 4);

	*reinterpret_cast<uintptr_t*>(jit) = reinterpret_cast<uintptr_t>(_fakeJITVtable);

	if (VanillaHooking::InstallHook(xorstr_("GetJitHook"), reinterpret_cast<uintptr_t>(getJit),
	                                reinterpret_cast<uintptr_t>(getJitHook), reinterpret_cast<uintptr_t*>(&oGetJit)) !=
		VanillaResult::Success)
		return false;

	return true;
	STR_ENCRYPT_END
	VM_LION_BLACK_END
}
