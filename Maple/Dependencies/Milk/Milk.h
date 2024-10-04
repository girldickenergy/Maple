#pragma once
#include "MilkMemory.h"
#include "Singleton.h"
#include "Structs/CRC.h"

#include <unordered_map>

struct someBassFuncRet
{
    void* esi;
    int ret;
};

struct v7fix
{
    uint8_t padding[0x40];
    double speed;
};

struct v8fix
{
    uint8_t padding[4];
    v7fix* v7;
};

struct v9fix
{
    uint8_t padding[0x40];
    float freq;
};

struct v10fix
{
    uint8_t padding[0x20];
    v9fix* v9;
    uint8_t padding2[0x90];
};

struct infoSectionStruct
{
    uint8_t padding[0x2F8];
    std::vector<float> rates;
    std::vector<uint32_t> spriteCollectionCounts;
    uint8_t padding2[0x9C];
    char o;
    char s;
    char u;
};

class Milk : public Singleton<Milk>
{
    MilkMemory _milkMemory;
    uintptr_t _authStubBaseAddress;
    DWORD _authStubSize;
    uintptr_t _firstCRCAddress;
    CRC* _firstCRC;
    std::unordered_map<uint32_t, std::unordered_map<uint32_t, uintptr_t>*>* _crcMap;
    std::vector<std::pair<uintptr_t, size_t>> _bypassed;
    uintptr_t _secondaryKey;

    infoSectionStruct* _infoSection;

    static inline uintptr_t _originalJITVtable;
    static inline uintptr_t* _copiedJITVtable;
    bool preparationSuccess;

    using fnGetJit = uintptr_t(__stdcall*)();
    static inline fnGetJit oGetJit;
    static uintptr_t __stdcall getJitHook();

    static inline struct v7fix v7 = {};
    static inline struct v8fix v8 = {};
    static inline struct v9fix v9 = {};
    static inline struct v10fix v10 = {};
    using fnSomeBassFunc = int(__stdcall*)(int handle);
    static inline fnSomeBassFunc oSomeBassFunc;

    __forceinline uintptr_t encryptValue(uintptr_t valuePointer, uintptr_t xorKey);
    __forceinline uintptr_t decryptValue(uintptr_t valuePointer, uintptr_t xorKey);

    uintptr_t findAuthStub();
    DWORD findAuthStubSize();
    uintptr_t findCRCMap();
    uintptr_t findSecondaryKey();
    uintptr_t findInfoSectionStruct();

    /**
     * \brief Bypasses the detection vector where functions would be checked against CRC32.
     */
    void doCRCBypass(uintptr_t address);

public:
    Milk(singletonLock);
    ~Milk();

    bool Prepare();
    bool DoCRCBypass(uintptr_t address);
    void HookJITVtable(int index, uintptr_t detour, uintptr_t* originalFunction);
    void SetSpriteCollectionCounts(uint32_t value);
    void AdjustRate(double rateMultiplier, bool isNoMod = false);
    void AdjustPollingVectorsToRate(double rateMultiplier);
    bool IsBroken();

    static someBassFuncRet* __stdcall SpoofPlaybackRate(int handle, DWORD ebp, DWORD ret);
};