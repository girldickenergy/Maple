#pragma once

#include <vector>
#include <mutex>

#include "CLR/CLRString.h"
#include "VanillaResult.h"
#include "Hooking/HookManager.h"
#include "PatternScanning/PatternScanner.h"

class Vanilla
{
    static inline std::mutex jitMutex;

    typedef uintptr_t(__thiscall* fnMakeJitWorker)(void* methodDesc, void* ilHeader, int flags1, int flags2);
    static inline fnMakeJitWorker oMakeJitWorker;
    static uintptr_t __fastcall MakeJitWorkerHook(void* methodDesc, void* unused, void* ilHeader, int flags1, int flags2);

    typedef void(__thiscall* fnGenGenerateCode)(void* compiler, uintptr_t* codePtr, size_t* nativeSizeOfCode);
    static inline fnGenGenerateCode oGenGenerateCode;
    static void __fastcall GenGenerateCodeHook(void* compiler, void* unused, uintptr_t* codePtr, size_t* nativeSizeOfCode);

    typedef void(__cdecl* fnJITCallback)(void* methodDesc, uintptr_t functionAddress, size_t functionSize);

    static inline std::vector<std::reference_wrapper<std::uintptr_t>> Relocations;
    static inline std::mutex relocationMutex;

    typedef void(__stdcall* fnRelocateAddress)(uint8_t** block);
    static inline fnRelocateAddress oRelocateAddress;
    static void __stdcall RelocateAddressHook(uint8_t** block);

    typedef void*(__fastcall* fnEntry2MethodDesc)(void* entryPoint, void* methodTable);
    static inline fnEntry2MethodDesc entry2MethodDesc;

    typedef uintptr_t(__thiscall* fnGetAddrOfSlot)(void* methodDesc);
    static inline fnGetAddrOfSlot getAddrOfSlot;

    typedef CLRString*(__cdecl* fnAllocateCLRString)(const wchar_t* pwsz);
    static inline fnAllocateCLRString allocateCLRString;

    static inline uintptr_t setCLRStringAddress;

    static inline bool usingCLR = false;
    static inline fnJITCallback jitCallback = nullptr;

    HookManager m_HookManager;
    PatternScanner m_PatternScanner;

public:
    Vanilla() = default;

    /**
     * \brief Initializes Vanilla for future use
     * \param useCLR Whether or not Vanilla should use CLR features
     * \return Result code
     */
    VanillaResult Initialize(bool useCLR = false);

    /**
     * \return The hook manager
     */
    HookManager& GetHookManager();
    /**
     * \return The pattern scanner
     */
    PatternScanner& GetPatternScanner();

    /**
     * \brief Sets a callback that will be called each time Just-In-Time compilation happens
     * \param callback Callback to set
     */
    void SetJITCallback(fnJITCallback callback);
    /**
     * \brief Removes Just-In-Time compilation callback
     */
    void RemoveJITCallback();

    /**
     * \brief Adds an object to the relocation watcher
     * \param relocation Address of an object to add
     */
    void AddRelocation(std::reference_wrapper<std::uintptr_t> relocation);
    /**
     * \brief Removes an object from the relocation watcher
     * \param relocation Address of an object to remove
     */
    void RemoveRelocation(std::reference_wrapper<std::uintptr_t> relocation);

    /**
     * \brief Returns method descriptor by method's native code address
     * \param nativeCodeAddress Native code address
     * \return A method descriptor
     */
    void* GetMethodDesc(uintptr_t nativeCodeAddress);

    /**
     * \brief Returns a slot address for the given method. 'Slot' refers to a memory location where the native code address is stored.
     * \param methodDesc Method descriptor
     * \return A slot address
     */
    uintptr_t GetMethodSlotAddress(void* methodDesc);

    /**
     * \brief Allocates a new CLR-compliant string
     * \param pwsz String to allocate
     * \return Pointer to allocated CLR-compliant string
     */
    CLRString* AllocateCLRString(const wchar_t* pwsz);
    /**
     * \brief Sets the value of a specified string field and its GC flag.
     * \param address Address of a string field.
     * \param string A string value to set.
     * \return True if the GC flag has been set, false otherwise.
     */
    bool SetCLRString(uintptr_t address, CLRString* string);
};
