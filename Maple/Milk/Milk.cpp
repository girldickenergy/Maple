#include "Milk.h"

#if __has_include("xorstr.h")
#include "xorstr.h"
#else
#define xorstr_(string) string
#endif

#include "../../Logging/Logger.h"

#include "../../SDK/Audio/AudioEngine.h"
#include "crc.h"

#include <ranges>

Milk::Milk()
{
    BassInternalDataStruct = BassInternalData();

    m_MilkMemory = MilkMemory();
    AuthStubBaseAddress = 0x00000000;
    m_FirstCRCAddress = 0x00000000;
    m_PreparationSuccess = false;

    m_PatternScanner = PatternScanner();
    m_HookManager = HookManager();
}

uintptr_t __stdcall Milk::GetJitHook()
{
    constexpr uint32_t STUB_SIZE = 0x7F5000;
    constexpr uint32_t BUFFER = 0x1000;

    const auto returnAddress = reinterpret_cast<uintptr_t>(__builtin_return_address(0));

    if (returnAddress > AuthStubBaseAddress && returnAddress < AuthStubBaseAddress + STUB_SIZE + BUFFER)
        return reinterpret_cast<uintptr_t>(&OriginalJITVTable);

    [[clang::musttail]] return oGetJit();
}

void Milk::SomeBassFuncHook()
{
    // TODO: add documentation to each line
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

uintptr_t Milk::EncryptValue(const uintptr_t valuePointer, uintptr_t xorKey)
{
    uintptr_t result = valuePointer;
    for (int i = 0; i < sizeof(uint32_t); i++)
    {
        const auto resultPointer = reinterpret_cast<uint8_t*>(&result) + i;
        *resultPointer = *resultPointer ^ *(reinterpret_cast<uint8_t*>(&xorKey) + i % 4);
        *resultPointer = *resultPointer + *(reinterpret_cast<uint8_t*>(&m_SecondaryKey) + i % 4);
    }

    return result;
}

uintptr_t Milk::DecryptValue(const uintptr_t valuePointer, uintptr_t xorKey)
{
    uintptr_t result = valuePointer;
    for (int i = 0; i < sizeof(uint32_t); i++)
    {
        const auto resultPointer = reinterpret_cast<uint8_t*>(&result) + i;
        *resultPointer = *resultPointer - *(reinterpret_cast<uint8_t*>(&m_SecondaryKey) + i % 4);
        *resultPointer = *resultPointer ^ *(reinterpret_cast<uint8_t*>(&xorKey) + i % 4);
    }

    return result;
}

uintptr_t Milk::FindAuthStub()
{
    for (const auto& region : *m_MilkMemory.GetMemoryRegions())
        if (region.State != MEM_FREE && region.Protect == PAGE_EXECUTE)
            return region.BaseAddress;

    return 0;
}

DWORD Milk::FindAuthStubSize()
{
    DWORD size = 0;

    for (const auto& region : *m_MilkMemory.GetMemoryRegions())
    {
        if (region.BaseAddress > AuthStubBaseAddress &&
            (region.State == MEM_FREE || (region.Protect != PAGE_EXECUTE && region.Protect != PAGE_EXECUTE_READWRITE)))
            return size;

        size += region.RegionSize;
    }

    return 0;
}

uintptr_t Milk::FindCRCMap()
{
    const auto pattern = xorstr_("55 8B EC 51 8D 45 FF 50 B9");

    for (const auto& region : *m_MilkMemory.GetMemoryRegions())
    {
        if (region.BaseAddress < AuthStubBaseAddress)
            continue;

        const uintptr_t result = m_PatternScanner.FindPatternInRange(pattern, AuthStubBaseAddress, m_AuthStubSize) + 0x09;

        if (result > AuthStubBaseAddress)
            return *reinterpret_cast<uintptr_t*>(result);
    }

    return 0;
}

uintptr_t Milk::FindSecondaryKey()
{
    const auto pattern = xorstr_("55 8B EC B8 ?? ?? ?? ?? E8 ?? ?? ?? ?? 89 ?? ?? E8 ?? ?? ?? ?? 89 ?? ?? C7");

    for (const auto& region : *m_MilkMemory.GetMemoryRegions())
    {
        if (region.BaseAddress < AuthStubBaseAddress)
            continue;

        const uintptr_t result = m_PatternScanner.FindPatternInRange(pattern, AuthStubBaseAddress, m_AuthStubSize);

        if (result > AuthStubBaseAddress)
            return result;
    }

    return 0;
}

bool Milk::DoCRCBypass(const uintptr_t address)
{
    if (!m_PreparationSuccess)
        return false;

    // TODO: maybe use ranges::any_of here?
    for (const auto& crcStruct : *m_CRCMap | std::views::values)
    {
        [[clang::always_inline]] const auto functionPointer = DecryptValue(crcStruct->FunctionPointer, crcStruct->FunctionPointerXORKey);
        [[clang::always_inline]] const auto functionSize = DecryptValue(crcStruct->FunctionSize, crcStruct->FunctionSizeXORKey);

        if (address >= functionPointer && address <= functionPointer + functionSize)
        {
            // TODO: import cryptopp
            // CryptoPP::CRC32 crc;
            // byte digest[CryptoPP::CRC32::DIGESTSIZE];
            // crc.CalculateDigest(digest, reinterpret_cast<byte*>(functionPointer), functionSize);

            // auto checksum = *reinterpret_cast<unsigned*>(digest) ^ 0xFFFFFFFF;
            // [[clang::always_inline]] crcStruct->Checksum = EncryptValue(checksum, crcStruct->ChecksumXORKey);

            return true;
        }
    }

    return false;
}

void Milk::HookJITVTable(const int index, const uintptr_t detour, uintptr_t* originalFunction)
{
    *originalFunction = CopiedJITVTable[index];
    CopiedJITVTable[index] = detour;
}

bool Milk::Prepare()
{
    AuthStubBaseAddress = FindAuthStub();
    if (!AuthStubBaseAddress)
        return false;

    m_AuthStubSize = FindAuthStubSize();
    if (!m_AuthStubSize)
        return false;

    m_FirstCRCAddress = FindCRCMap();
    if (!m_FirstCRCAddress)
        return false;

    m_SecondaryKey = FindSecondaryKey();
    if (!m_SecondaryKey)
        return false;

    m_CRCMap = reinterpret_cast<std::map<uint32_t, CRC*>*>(m_FirstCRCAddress);

    const auto firstCRC = m_CRCMap->begin()->second;

    if ([[clang::always_inline]] const auto decryptedSize = DecryptValue(firstCRC->FunctionSize, firstCRC->FunctionSizeXORKey);
        decryptedSize < 1 || decryptedSize > 2000)
        return false;

    void* getJit = GetProcAddress(GetModuleHandleA(xorstr_("clrjit.dll")), xorstr_("getJit"));
    if (!getJit)
        return false;

    const uintptr_t jit = static_cast<fnGetJit>(getJit)();
    if (!jit)
        return false;

    OriginalJITVTable = *reinterpret_cast<uintptr_t*>(jit);
    if (!OriginalJITVTable)
        return false;

    CopiedJITVTable = new uintptr_t[7];
    memcpy(CopiedJITVTable, reinterpret_cast<void*>(OriginalJITVTable), 7 * 4);

    *reinterpret_cast<uintptr_t*>(jit) = reinterpret_cast<uintptr_t>(CopiedJITVTable);

    if (m_HookManager.InstallHook(xorstr_("GetJitHook"), reinterpret_cast<uintptr_t>(getJit), reinterpret_cast<uintptr_t>(GetJitHook),
                                  reinterpret_cast<uintptr_t*>(&oGetJit)) != VanillaResult::Success)
        return false;

    const uintptr_t someBassFunc = m_PatternScanner.FindPatternInModule(xorstr_("55 8B EC F7 45 08"), xorstr_("bass.dll"));
    if (!someBassFunc)
        return false;

    if (m_HookManager.InstallHook(xorstr_("SomeBassFunc"), someBassFunc, reinterpret_cast<uintptr_t>(SomeBassFuncHook),
                                  reinterpret_cast<uintptr_t*>(&oSomeBassFunc)) != VanillaResult::Success)
        return false;

    m_PreparationSuccess = true;

    return true;
}

int __stdcall Milk::SpoofPlaybackRate(const int handle, const DWORD stackBasePointer, const DWORD returnAddress)
{
    const auto val = oSomeBassFunc(handle);

    constexpr uint32_t STUB_SIZE = 0x7F5000;
    constexpr uint32_t BUFFER = 0x1000;

    if (returnAddress > AuthStubBaseAddress && returnAddress < AuthStubBaseAddress + STUB_SIZE + BUFFER)
    {
        const auto var_ptr = reinterpret_cast<BassInternalData::V8Fix**>(stackBasePointer - 0x28);

        BassInternalDataStruct.V8.V7 = &BassInternalDataStruct.V7;
        // TODO: implement
        //  V7.Speed = AudioEngine::GetModTempo(); // fix Speed

        *var_ptr = &BassInternalDataStruct.V8;

        auto freq = reinterpret_cast<BassInternalData::V10Fix*>(val)->V9->Frequency; // get current frequency

        _InterlockedExchangeAdd(reinterpret_cast<volatile unsigned __int32*>(val + 164), 0xFFFFFFFF);

        BassInternalDataStruct.V10.V9 = &BassInternalDataStruct.V9;
        // TODO: implement
        // V9.Frequency = AudioEngine::GetModFrequency(Frequency); // fix Frequency

        return reinterpret_cast<int>(&BassInternalDataStruct.V10);
    }

    return val;
}