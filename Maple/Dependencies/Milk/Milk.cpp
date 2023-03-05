#include "Milk.h"

#include <intrin.h>

#include "../Vanilla/PatternScanning/VanillaPatternScanner.h"
#include "../../Utilities/Security/xorstr.hpp"
#include "../../Logging/Logger.h"
#include "../../Utilities/Security/Security.h"
#include "../../SDK/Audio/AudioEngine.h"
#include <ThemidaSDK.h>
#include <Hooking/VanillaHooking.h>

#include "crc.h"

#pragma optimize("", off)

Milk::Milk(singletonLock)
{
	VM_FISH_RED_START

	_milkMemory = MilkMemory();
	_authStubBaseAddress = 0x00000000;
	_firstCRCAddress = 0x00000000;
	_firstCRC = nullptr;
	preparationSuccess = false;

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
	bool isAuthCall = retAddress > Get()._authStubBaseAddress && retAddress < Get()._authStubBaseAddress + STUB_SIZE + BUFFER;

	if (isAuthCall)
		return reinterpret_cast<uintptr_t>(&_originalJITVtable);

	return oGetJit();
}

void _declspec(naked) Milk::someBassFuncHook()
{
	_asm
	{
		mov eax, ebp
		mov ecx, dword ptr[esp]
		push ebp
		mov ebp, esp
		push ecx
		push eax
		mov ecx, dword ptr[ebp + 8]
		push ecx
		call spoofPlaybackRate
		pop ebp
		retn 4
	}
}

int __stdcall Milk::spoofPlaybackRate(int handle, DWORD ebp, DWORD ret)
{
	auto val = oSomeBassFunc(handle);

	const uint32_t STUB_SIZE = 0x7F5000;
	const uint32_t BUFFER = 0x1000;

	bool isAuthCall = ret > Get()._authStubBaseAddress && ret < Get()._authStubBaseAddress + STUB_SIZE + BUFFER;

	if (isAuthCall)
	{
		auto var_ptr = (v8fix**)(ebp + 0x40);

		v8.v7 = &v7;
		v7.speed = AudioEngine::GetModTempo(); // fix speed

		*var_ptr = &v8;

		auto freq = ((v10fix*)val)->v9->freq; // get current frequency

		_InterlockedExchangeAdd((volatile unsigned __int32*)(val + 164), 0xFFFFFFFF);

		v10.v9 = &v9;
		v9.freq = AudioEngine::GetModFrequency(freq); // fix freq

		return (int)(&v10);
	}

	return val;
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

	auto pattern = xorstr_("55 8B EC B9 ?? ?? ?? ?? E8 ?? ?? ?? ?? 5D C3 CC 55");

	STR_ENCRYPT_END

	for (const auto& region : *_milkMemory.GetMemoryRegions())
	{
		if (region.BaseAddress < _authStubBaseAddress)
			continue;

		uintptr_t result = VanillaPatternScanner::FindPatternInRange(pattern, region.BaseAddress, region.RegionSize, 4);

		if (result > _authStubBaseAddress)
			return result;
	}

	return 0;

	VM_LION_BLACK_END
}

void Milk::doCRCBypass(uintptr_t address)
{
	VM_LION_BLACK_START

	CRC* currentCRCStruct = _firstCRC;
	while (currentCRCStruct)
	{
		if (address >= reinterpret_cast<uintptr_t>(currentCRCStruct->functionPointer) && address <= reinterpret_cast<uintptr_t>(currentCRCStruct->functionPointer) + currentCRCStruct->functionSize)
		{
			CryptoPP::CRC32 crc;
			byte digest[CryptoPP::CRC32::DIGESTSIZE];
			crc.CalculateDigest(digest, static_cast<byte*>(currentCRCStruct->functionPointer), currentCRCStruct->functionSize);

			currentCRCStruct->checksum = *reinterpret_cast<unsigned*>(digest) ^ 0xFFFFFFFF;

			return;
		}

		currentCRCStruct = currentCRCStruct->nextEntry;
	}

	VM_LION_BLACK_END
}

bool Milk::DoCRCBypass(uintptr_t address)
{
	VM_LION_BLACK_START
	STR_ENCRYPT_START

	if (!preparationSuccess)
		return false;

	doCRCBypass(address);

	return true;

	STR_ENCRYPT_END
	VM_LION_BLACK_END
}

void Milk::HookJITVtable(int index, uintptr_t detour, uintptr_t* originalFunction)
{
	*originalFunction = _copiedJITVtable[index];
	_copiedJITVtable[index] = detour;
}

bool Milk::Prepare()
{
	VM_LION_BLACK_START
	STR_ENCRYPT_START

	_authStubBaseAddress = findAuthStub();
	if (!_authStubBaseAddress)
		return false;

	Logger::Log(LogSeverity::Debug, xorstr_("[Milk] AS != 0x00000000"));

	_firstCRCAddress = findFirstCRCAddress();
	if (!_firstCRCAddress)
		return false;

	Logger::Log(LogSeverity::Debug, xorstr_("[Milk] FC != 0x00000000"));

	_firstCRC = **reinterpret_cast<CRC***>(_firstCRCAddress);

	Logger::Log(LogSeverity::Debug, _firstCRC->className);
	Logger::Log(LogSeverity::Debug, _firstCRC->functionName);
	Logger::Log(LogSeverity::Debug, std::to_string(_firstCRC->functionSize).c_str());

	if (_firstCRC->functionSize < 1 || _firstCRC->functionSize > 2000)
		return false;

	Logger::Log(LogSeverity::Debug, xorstr_("[Milk] FC FS OK"));

	void* getJit = GetProcAddress(GetModuleHandleA(xorstr_("clrjit.dll")), xorstr_("getJit"));
	if (!getJit)
		return false;

	Logger::Log(LogSeverity::Debug, xorstr_("[Milk] GJ != 0x00000000"));

	uintptr_t jit = static_cast<fnGetJit>(getJit)();
	if (!jit)
		return false;

	Logger::Log(LogSeverity::Debug, xorstr_("[Milk] J != 0x00000000"));

	_originalJITVtable = *reinterpret_cast<uintptr_t*>(jit);
	if (!_originalJITVtable)
		return false;

	Logger::Log(LogSeverity::Debug, xorstr_("[Milk] JVMT != 0x00000000"));

	_copiedJITVtable = new uintptr_t[7];
	memcpy(_copiedJITVtable, reinterpret_cast<void*>(_originalJITVtable), 7 * 4);

	*reinterpret_cast<uintptr_t*>(jit) = reinterpret_cast<uintptr_t>(_copiedJITVtable);

	if (VanillaHooking::InstallHook(xorstr_("GetJitHook"), reinterpret_cast<uintptr_t>(getJit), reinterpret_cast<uintptr_t>(getJitHook), reinterpret_cast<uintptr_t*>(&oGetJit)) != VanillaResult::Success)
		return false;

	Logger::Log(LogSeverity::Debug, xorstr_("[Milk] GJH OK"));

	uintptr_t someBassFunc = VanillaPatternScanner::FindPatternInModule(xorstr_("55 8B EC F7 45 08"), xorstr_("bass.dll"));
	if (!someBassFunc)
		return false;

	Logger::Log(LogSeverity::Debug, xorstr_("[Milk] SBF != 0x00000000"));

	if (VanillaHooking::InstallHook(xorstr_("SomeBassFunc"), someBassFunc, reinterpret_cast<uintptr_t>(someBassFuncHook), reinterpret_cast<uintptr_t*>(&oSomeBassFunc)) != VanillaResult::Success)
		return false;

	Logger::Log(LogSeverity::Debug, xorstr_("[Milk] SBFH OK"));

	preparationSuccess = true;

	return true;

	STR_ENCRYPT_END
	VM_LION_BLACK_END
}
