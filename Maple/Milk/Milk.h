#pragma once
#include "CRC.h"
#include "MilkMemory.h"

#include <map>

#include "../Dependencies/Vanilla/PatternScanning/PatternScanner.h"

#include "../Dependencies/Vanilla/Hooking/HookManager.h"

struct BassInternalData
{
    struct V7Fix
    {
        uint8_t padding[0x40];
        double Speed;
    };

    struct V8Fix
    {
        uint8_t padding[4];
        V7Fix* V7;
    };

    struct V9Fix
    {
        uint8_t padding[0x40];
        float Frequency;
    };

    struct V10Fix
    {
        uint8_t padding[0x20];
        V9Fix* V9;
        uint8_t padding2[0x90];
    };

    V7Fix V7;
    V8Fix V8;
    V9Fix V9;
    V10Fix V10;

    BassInternalData()
    {
        V7 = {};
        V8 = {};
        V9 = {};
        V10 = {};
    }
};

class Milk
{
    static inline uintptr_t AuthStubBaseAddress;
    // TODO: find better name for this
    static inline BassInternalData BassInternalDataStruct;

    PatternScanner m_PatternScanner;
    HookManager m_HookManager;

    MilkMemory m_MilkMemory;
    DWORD m_AuthStubSize;
    uintptr_t m_FirstCRCAddress;
    std::map<uint32_t, CRC*>* m_CRCMap;
    uintptr_t m_SecondaryKey;

    static inline uintptr_t OriginalJITVTable;
    static inline uintptr_t* CopiedJITVTable;
    bool m_PreparationSuccess;

    using fnGetJit = uintptr_t(__stdcall*)();
    static inline fnGetJit oGetJit;
    static uintptr_t __stdcall GetJitHook();

    using fnSomeBassFunc = int(__stdcall*)(int handle);
    static inline fnSomeBassFunc oSomeBassFunc;

    static void _declspec(naked) SomeBassFuncHook();

    uintptr_t EncryptValue(uintptr_t valuePointer, uintptr_t xorKey);
    uintptr_t DecryptValue(uintptr_t valuePointer, uintptr_t xorKey);

    uintptr_t FindAuthStub();
    DWORD FindAuthStubSize();
    uintptr_t FindCRCMap();
    uintptr_t FindSecondaryKey();

public:
    Milk();

    bool Prepare();
    bool DoCRCBypass(uintptr_t address);
    void HookJITVTable(int index, uintptr_t detour, uintptr_t* originalFunction);

    static int __stdcall SpoofPlaybackRate(int handle, DWORD stackBasePointer, DWORD returnAddress);
};