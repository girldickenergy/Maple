#include "Milk.h"

#include <intrin.h>
#include <xorstr.hpp>

#include "../../Logging/Logger.h"
#include "../Vanilla/PatternScanning/VanillaPatternScanner.h"

#include <Hooking/VanillaHooking.h>
#include <VirtualizerSDK.h>

#include "../../SDK/Audio/AudioEngine.h"
#include "crc.h"

#pragma clang optimize off

Milk::Milk(singletonLock)
{
    VIRTUALIZER_FISH_RED_START

    _milkMemory = MilkMemory();
    _authStubBaseAddress = 0x00000000;
    _firstCRCAddress = 0x00000000;
    _firstCRC = nullptr;
    preparationSuccess = false;

    VIRTUALIZER_FISH_RED_END
}

Milk::~Milk()
{
    VIRTUALIZER_FISH_RED_START
    _milkMemory.~MilkMemory();
    VIRTUALIZER_FISH_RED_END
}

uintptr_t __stdcall Milk::getJitHook()
{
    const uint32_t STUB_SIZE = 0x7F5000;
    const uint32_t BUFFER = 0x1000;

    auto retAddress = reinterpret_cast<uintptr_t>(__builtin_return_address(0));
    bool isAuthCall = retAddress > Get()._authStubBaseAddress && retAddress < Get()._authStubBaseAddress + STUB_SIZE + BUFFER;

    if (isAuthCall)
        return reinterpret_cast<uintptr_t>(&_originalJITVtable);

    [[clang::musttail]] return oGetJit();
}

void _declspec(naked) someBassFuncHook()
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
		call Milk::SpoofPlaybackRate
		pop ebp
		retn 4
    }
}

uintptr_t Milk::encryptValue(uintptr_t valuePointer, uintptr_t xorKey)
{
    uintptr_t result = valuePointer;
    for (int i = 0; i < sizeof(uint32_t); i++)
    {
        auto resultPointer = reinterpret_cast<uint8_t*>(&result) + i;
        *resultPointer = *resultPointer ^ *(reinterpret_cast<uint8_t*>(&xorKey) + i % 4);
        *resultPointer = *resultPointer + *(reinterpret_cast<uint8_t*>(&_secondaryKey) + i % 4);
    }

    return result;
}

uintptr_t Milk::decryptValue(uintptr_t valuePointer, uintptr_t xorKey)
{
    uintptr_t result = valuePointer;
    for (int i = 0; i < sizeof(uint32_t); i++)
    {
        auto resultPointer = reinterpret_cast<uint8_t*>(&result) + i;
        *resultPointer = *resultPointer - *(reinterpret_cast<uint8_t*>(&_secondaryKey) + i % 4);
        *resultPointer = *resultPointer ^ *(reinterpret_cast<uint8_t*>(&xorKey) + i % 4);
    }

    return result;
}

uintptr_t Milk::findAuthStub()
{
    VIRTUALIZER_LION_BLACK_START

    for (const auto& region : *_milkMemory.GetMemoryRegions())
        if (region.State != MEM_FREE && region.Protect == PAGE_EXECUTE)
            return region.BaseAddress;

    VIRTUALIZER_LION_BLACK_END

    return 0;
}

DWORD Milk::findAuthStubSize()
{
    DWORD size = 0;

    for (const auto& region : *_milkMemory.GetMemoryRegions())
    {
        if (region.BaseAddress > _authStubBaseAddress && (region.State == MEM_FREE || (region.Protect != PAGE_EXECUTE && region.Protect != PAGE_EXECUTE_READWRITE)))
            return size;

	size += region.RegionSize;
    }

    return 0;
}

// ReSharper disable once CppInconsistentNaming
uintptr_t Milk::findCRCMap()
{
    VIRTUALIZER_LION_BLACK_START

    auto pattern = xorstr_("FF 5D C3 55 8B EC B9 ?? ?? ?? ?? E8 ?? ?? ?? ?? 68 ?? ?? ?? ?? E8 ?? ?? ?? ?? 59 5D C3 55 8B");

    for (const auto& region : *_milkMemory.GetMemoryRegions())
    {
        if (region.BaseAddress < _authStubBaseAddress)
            continue;

        uintptr_t result = VanillaPatternScanner::FindPatternInRange(pattern, _authStubBaseAddress, _authStubSize, 7);

        if (result > _authStubBaseAddress)
            return *reinterpret_cast<uintptr_t*>(result);
    }

    VIRTUALIZER_LION_BLACK_END

    return 0;
}

uintptr_t Milk::findSecondaryKey()
{
    VIRTUALIZER_LION_BLACK_START

    auto pattern = xorstr_("55 8B EC B8 ?? ?? ?? ?? E8 ?? ?? ?? ?? 89");

    for (const auto& region : *_milkMemory.GetMemoryRegions())
    {
        if (region.BaseAddress < _authStubBaseAddress)
            continue;

        uintptr_t result = VanillaPatternScanner::FindPatternInRange(pattern, _authStubBaseAddress, _authStubSize);

        if (result > _authStubBaseAddress)
            return result;
    }

    VIRTUALIZER_LION_BLACK_END

    return 0;
}

void Milk::doCRCBypass(uintptr_t address)
{
    VIRTUALIZER_TIGER_WHITE_START

    for (const auto& pair : _bypassed)
    {
        if (address >= pair.first && address <= pair.first + pair.second)
            return;
    }

    for (const auto& pair : *_crcMap)
    {
        auto functionPointerStruct = (*pair.second)[reinterpret_cast<uintptr_t>(pair.second) ^ 0x48AB2731];
        auto functionSizeStruct = (*pair.second)[reinterpret_cast<uintptr_t>(pair.second) ^ 0x13E76EB2];

        auto functionPointer = decryptValue(*reinterpret_cast<uintptr_t*>(functionPointerStruct), *reinterpret_cast<uintptr_t*>(functionPointerStruct + 0x4));
        auto functionSize = decryptValue(*reinterpret_cast<uintptr_t*>(functionSizeStruct), *reinterpret_cast<uintptr_t*>(functionSizeStruct + 0x4));

        if (address >= functionPointer && address <= functionPointer + functionSize)
        {
            uintptr_t copiedFunc = reinterpret_cast<uintptr_t>(malloc(functionSize));
            memcpy(reinterpret_cast<void*>(copiedFunc), reinterpret_cast<void*>(functionPointer), functionSize);
            *reinterpret_cast<uintptr_t*>(functionPointerStruct) = encryptValue(copiedFunc, *reinterpret_cast<uintptr_t*>(functionPointerStruct + 0x4));

            _bypassed.emplace_back(functionPointer, functionSize);

            return;
        }
    }

    VIRTUALIZER_TIGER_WHITE_END
}

bool Milk::DoCRCBypass(uintptr_t address)
{
    VIRTUALIZER_TIGER_WHITE_START

#ifdef NO_BYPASS
        VIRTUALIZER_TIGER_BLACK_END
        return true;
#endif

    if (!preparationSuccess)
        return false;

    doCRCBypass(address);

    VIRTUALIZER_TIGER_WHITE_END

    return true;
}

void Milk::HookJITVtable(int index, uintptr_t detour, uintptr_t* originalFunction)
{
    *originalFunction = _copiedJITVtable[index];
    _copiedJITVtable[index] = detour;
}

bool Milk::Prepare()
{
    VIRTUALIZER_LION_BLACK_START

#ifdef NO_BYPASS
        VIRTUALIZER_LION_BLACK_END
        return true;
#endif
    Logger::StartPerformanceCounter(xorstr_("{99D6FB11-046C-4ACB-A269-92B179C3186A}"));

    _authStubBaseAddress = findAuthStub();
    if (!_authStubBaseAddress)
        return false;

    Logger::Log(LogSeverity::Debug, xorstr_("[Milk] AS != 0x00000000"));

    _authStubSize = findAuthStubSize();
    if (!_authStubSize)
        return false;

    Logger::Log(LogSeverity::Debug, xorstr_("[Milk] ASS != 0x00000000"));

    _firstCRCAddress = findCRCMap();
    if (!_firstCRCAddress)
        return false;

    Logger::Log(LogSeverity::Debug, xorstr_("[Milk] FC != 0x00000000"));

    _secondaryKey = findSecondaryKey();
    if (!_secondaryKey)
        return false;

    Logger::Log(LogSeverity::Debug, xorstr_("[Milk] SC != 0x00000000"));

    _crcMap = reinterpret_cast<std::unordered_map<uint32_t, std::unordered_map<uint32_t, uintptr_t>*>*>(_firstCRCAddress);

    //_firstCRC = _crcMap->begin()->second;
    //auto decryptedSize = decryptValue(_firstCRC->functionSize, _firstCRC->functionSizeXORKey);

    //Logger::Log(LogSeverity::Debug, reinterpret_cast<char*>(decryptValue(_firstCRC->className, _firstCRC->classNameXORKey)));
    //Logger::Log(LogSeverity::Debug, reinterpret_cast<char*>(decryptValue(_firstCRC->functionName, _firstCRC->functionNameXORKey)));
    //Logger::Log(LogSeverity::Debug, std::to_string(decryptedSize).c_str());

    //if (decryptedSize < 1 || decryptedSize > 2000)
    //    return false;

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

    if (VanillaHooking::InstallHook(xorstr_("GetJitHook"), reinterpret_cast<uintptr_t>(getJit), reinterpret_cast<uintptr_t>(getJitHook),
        reinterpret_cast<uintptr_t*>(&oGetJit)) != VanillaResult::Success)
        return false;

    Logger::Log(LogSeverity::Debug, xorstr_("[Milk] GJH OK"));

    uintptr_t someBassFunc = VanillaPatternScanner::FindPatternInModule(xorstr_("55 8B EC F7 45 08"), xorstr_("bass.dll"));
    if (!someBassFunc)
        return false;

    Logger::Log(LogSeverity::Debug, xorstr_("[Milk] SBF != 0x00000000"));

    if (VanillaHooking::InstallHook(xorstr_("SomeBassFunc"), someBassFunc, reinterpret_cast<uintptr_t>(someBassFuncHook),
        reinterpret_cast<uintptr_t*>(&oSomeBassFunc)) != VanillaResult::Success)
        return false;

    Logger::Log(LogSeverity::Debug, xorstr_("[Milk] SBFH OK"));

    preparationSuccess = true;

    Logger::StopPerformanceCounter(xorstr_("{99D6FB11-046C-4ACB-A269-92B179C3186A}"));
    VIRTUALIZER_LION_BLACK_END

    return true;
}

int __stdcall Milk::SpoofPlaybackRate(int handle, DWORD ebp, DWORD ret)
{
    auto val = oSomeBassFunc(handle);

    const uint32_t STUB_SIZE = 0x7F5000;
    const uint32_t BUFFER = 0x1000;

    bool isAuthCall = ret > Get()._authStubBaseAddress && ret < Get()._authStubBaseAddress + STUB_SIZE + BUFFER;

    if (isAuthCall)
    {
        auto var_ptr = (v8fix**)(ebp - 0x28);

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

#pragma clang optimize on