#include "Milk.h"

#include <intrin.h>
#include <xorstr.hpp>

#include "../../Logging/Logger.h"
#include "../Vanilla/PatternScanning/VanillaPatternScanner.h"

#include <Hooking/VanillaHooking.h>
#include <VirtualizerSDK.h>

#include "../../SDK/Audio/AudioEngine.h"
#include "../../SDK/Player/Player.h"
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
    const uint32_t STUB_SIZE = 0x17D7840;
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
		mov eax, esi
		mov ecx, dword ptr[esp]
		push ebp
		mov ebp, esp
		push ecx
		push eax
		mov ecx, dword ptr[ebp + 8]
		push ecx
		call Milk::SpoofPlaybackRate
        mov esi, [eax]
        mov eax, [eax + 4]
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
    uintptr_t ret;
    VIRTUALIZER_LION_BLACK_START

    ret = *reinterpret_cast<uintptr_t*>(_authStubBaseAddress + 0x4E);

    VIRTUALIZER_LION_BLACK_END

    return ret;
}

uintptr_t Milk::findSecondaryKey()
{
    VIRTUALIZER_LION_BLACK_START

    auto pattern = xorstr_("B8 ?? ?? ?? ?? E8 ?? ?? ?? ?? E8 ?? ?? ?? ?? 50");

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

uintptr_t Milk::findInfoSectionStruct()
{
    VIRTUALIZER_LION_BLACK_START

    auto pattern = xorstr_("0C 89 35 ?? ?? ?? ?? 8B CE E8");

    for (const auto& region : *_milkMemory.GetMemoryRegions())
    {
        if (region.BaseAddress < _authStubBaseAddress)
            continue;

        uintptr_t result = VanillaPatternScanner::FindPatternInRange(pattern, _authStubBaseAddress, _authStubSize);

        if (result > _authStubBaseAddress)
            return *reinterpret_cast<uintptr_t*>(result + 0x3);
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
        auto functionPointerStruct = (*pair.second)[reinterpret_cast<uintptr_t>(pair.second) ^ 0x36ef87db];
        auto functionSizeStruct = (*pair.second)[reinterpret_cast<uintptr_t>(pair.second) ^ 0x4f233a8e];
        auto methodInfoStruct = (*pair.second)[reinterpret_cast<uintptr_t>(pair.second) ^ 0x53ff6340];

        auto functionPointer = decryptValue(*reinterpret_cast<uintptr_t*>(functionPointerStruct), *reinterpret_cast<uintptr_t*>(functionPointerStruct + 0x4));
        auto functionSize = decryptValue(*reinterpret_cast<uintptr_t*>(functionSizeStruct), *reinterpret_cast<uintptr_t*>(functionSizeStruct + 0x4));
        auto methodInfo = decryptValue(*reinterpret_cast<uintptr_t*>(methodInfoStruct), *reinterpret_cast<uintptr_t*>(methodInfoStruct + 0x4));

        if (address >= functionPointer && address <= functionPointer + functionSize)
        {
            auto copiedFunc = reinterpret_cast<uintptr_t>(VirtualAlloc(NULL, functionSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE));
            memcpy(reinterpret_cast<void*>(copiedFunc), reinterpret_cast<void*>(functionPointer), functionSize);
            *reinterpret_cast<uintptr_t*>(functionPointerStruct) = encryptValue(copiedFunc, *reinterpret_cast<uintptr_t*>(functionPointerStruct + 0x4));

            auto copiedMethodInfo = reinterpret_cast<uintptr_t>(malloc(0xC));
            memcpy(reinterpret_cast<void*>(copiedMethodInfo), reinterpret_cast<void*>(methodInfo), 0xC);
            *reinterpret_cast<uintptr_t*>(copiedMethodInfo + 0x8) = copiedFunc;
            *reinterpret_cast<uintptr_t*>(methodInfoStruct) = encryptValue(copiedMethodInfo, *reinterpret_cast<uintptr_t*>(methodInfoStruct + 0x4));

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

void Milk::SetSpriteCollectionCounts(uint32_t value)
{
    VIRTUALIZER_TIGER_WHITE_START

    for (unsigned int& spriteCollectionCount : _infoSection->spriteCollectionCounts)
	    spriteCollectionCount = value;

    VIRTUALIZER_TIGER_WHITE_END
}

void Milk::AdjustRate(double rateMultiplier)
{
    for (float& rate : _infoSection->rates)
        rate /= rateMultiplier;
}

void Milk::AdjustPollingVectorsToRate(double rateMultiplier)
{
    VIRTUALIZER_TIGER_WHITE_START

    size_t ratesRequiredSize = _infoSection->rates.size() * rateMultiplier;
    size_t spriteCollectionCountsRequiredSize = _infoSection->spriteCollectionCounts.size() * rateMultiplier;

    _infoSection->rates.resize(ratesRequiredSize, _infoSection->rates[0]);
    _infoSection->spriteCollectionCounts.resize(spriteCollectionCountsRequiredSize, _infoSection->spriteCollectionCounts[0]);

    VIRTUALIZER_TIGER_WHITE_END
}

bool Milk::IsBroken()
{
    return _infoSection->rates.empty() || (Player::GetPlayMode() != PlayModes::OsuMania && Player::GetPlayMode() != PlayModes::CatchTheBeat && _infoSection->spriteCollectionCounts.empty());
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

    Logger::Log(LogSeverity::Debug, xorstr_("[Milk] FC FS OK"));

    auto infoSectionPtr = findInfoSectionStruct();
    if (!infoSectionPtr)
        return false;

    Logger::Log(LogSeverity::Debug, xorstr_("[Milk] ISS != 0x00000000"));

    _infoSection = *reinterpret_cast<infoSectionStruct**>(infoSectionPtr);

    if (_infoSection->o != 'o' || _infoSection->s != 's' || _infoSection->u != 'u')
        return false;

    Logger::Log(LogSeverity::Debug, xorstr_("[Milk] IS OK"));

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

someBassFuncRet* __stdcall Milk::SpoofPlaybackRate(int handle, DWORD esi, DWORD ret)
{
    auto val = oSomeBassFunc(handle);

    someBassFuncRet retStruct;
    retStruct.esi = (void*)esi;
    retStruct.ret = val;

    const uint32_t STUB_SIZE = 0x17D7840;
    const uint32_t BUFFER = 0x1000;

    bool isAuthCall = ret > Get()._authStubBaseAddress && ret < Get()._authStubBaseAddress + STUB_SIZE + BUFFER;

    if (isAuthCall)
    {
        v8.v7 = &v7;
        v7.speed = AudioEngine::GetModTempo(); // fix speed

        auto freq = ((v10fix*)val)->v9->freq; // get current frequency

        _InterlockedExchangeAdd((volatile unsigned __int32*)(val + 164), 0xFFFFFFFF);

        v10.v9 = &v9;
        v9.freq = AudioEngine::GetModFrequency(freq); // fix freq

        retStruct.esi = &v8;
        retStruct.ret = (int)(&v10);

        return &retStruct;
    }

    return &retStruct;
}

#pragma clang optimize on