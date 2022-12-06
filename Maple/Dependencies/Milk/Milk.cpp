#include "Milk.h"

#include "../Vanilla/PatternScanning/VanillaPatternScanner.h"
#include "../../Utilities/Security/xorstr.hpp"
#include "../../Logging/Logger.h"
#include "../../Utilities/Security/Security.h"
#include <ThemidaSDK.h>

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

uintptr_t Milk::findAuthStub()
{
	VM_LION_BLACK_START
	for (auto const& region : *_milkMemory.GetMemoryRegions())
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
	for (auto const& region : *_milkMemory.GetMemoryRegions())
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
	
	return true;
	STR_ENCRYPT_END
	VM_LION_BLACK_END
}