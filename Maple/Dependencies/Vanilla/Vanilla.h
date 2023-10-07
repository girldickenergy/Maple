#pragma once

#include <vector>
#include <mutex>

#include "CLR/CLRString.h"
#include "VanillaResult.h"
#include "Hooking/HookManager.h"
#include "PatternScanning/PatternScanner.h"

class Vanilla
{
    typedef int(__stdcall* fnCompileMethod)(uintptr_t instance, uintptr_t compHnd, uintptr_t methodInfo, unsigned int flags, uintptr_t* entryAddress, unsigned int* nativeSizeOfCode);
    typedef void(__cdecl* fnJITCallback)(uintptr_t functionAddress, unsigned int functionSize);
    static inline fnCompileMethod oCompileMethod;
    static int __stdcall CompileMethodHook(uintptr_t instance, uintptr_t compHnd, uintptr_t methodInfo, unsigned int flags, uintptr_t* entryAddress, unsigned int* nativeSizeOfCode);

    static inline std::vector<std::reference_wrapper<std::uintptr_t>> Relocations;
    static inline std::mutex relocationMutex;
    typedef void(__stdcall* fnRelocateAddress)(uint8_t** block);
    static inline fnRelocateAddress oRelocateAddress;
    static void __stdcall RelocateAddressHook(uint8_t** block);

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
