#include "Vanilla.h"

#if __has_include("xorstr.h")
    #include "xorstr.h"
#else
    #define xorstr_(string) string
#endif

size_t functionSize = 0u;

uintptr_t __fastcall Vanilla::MakeJitWorkerHook(void* methodDesc, void* unused, void* ilHeader, int flags1, int flags2)
{
    uintptr_t functionAddress = oMakeJitWorker(methodDesc, ilHeader, flags1, flags2);

    if (functionAddress)
        jitCallback(methodDesc, functionAddress, functionSize);

    jitMutex.unlock();

    return functionAddress;
}

void __fastcall Vanilla::GenGenerateCodeHook(void* compiler, void* unused, uintptr_t* codePtr, size_t* nativeSizeOfCode)
{
    jitMutex.lock();

    oGenGenerateCode(compiler, codePtr, nativeSizeOfCode);

    functionSize = *nativeSizeOfCode;
}

void __stdcall Vanilla::RelocateAddressHook(uint8_t** block)
{
    if (*block != nullptr)
    {
        std::unique_lock lock(relocationMutex);

        for (auto& relocation : Relocations)
        {
            if (relocation == reinterpret_cast<uintptr_t>(*block))
            {
                oRelocateAddress(block);

                relocation.get() = reinterpret_cast<uintptr_t>(*block);
                return;
            }
        }
    }

    return oRelocateAddress(block);
}

VanillaResult Vanilla::Initialize(bool useCLR)
{
    usingCLR = useCLR;

    if (usingCLR)
    {
        const uintptr_t makeJitWorkerAddress = m_PatternScanner.FindPatternInModule(xorstr_("68 ?? ?? ?? ?? B8 ?? ?? ?? ?? E8 ?? ?? ?? ?? 8B F9 89 7D AC"), xorstr_("clr.dll"));
        if (!makeJitWorkerAddress)
            return VanillaResult::JITFailure;

        if (m_HookManager.InstallTrampolineHook(xorstr_("MethodDesc::MakeJitWorker"), makeJitWorkerAddress, reinterpret_cast<uintptr_t>(MakeJitWorkerHook), reinterpret_cast<uintptr_t*>(&oMakeJitWorker)) != VanillaResult::Success)
            return VanillaResult::JITFailure;

        const uintptr_t genGenerateCodeAddress = m_PatternScanner.FindPatternInModule(xorstr_("55 8B EC 83 EC 14 53 56 57 8B F1 E8"), xorstr_("clrjit.dll"));
        if (!genGenerateCodeAddress)
            return VanillaResult::JITFailure;

        if (m_HookManager.InstallTrampolineHook(xorstr_("Compiler::genGenerateCode"), genGenerateCodeAddress, reinterpret_cast<uintptr_t>(GenGenerateCodeHook), reinterpret_cast<uintptr_t*>(&oGenGenerateCode)) != VanillaResult::Success)
            return VanillaResult::JITFailure;

        const uintptr_t relocateAddressAddress = m_PatternScanner.FindPatternInModule(xorstr_("55 8B EC 57 8B 7D 08 8B 0F 3B 0D"), xorstr_("clr.dll"));
        if (!relocateAddressAddress)
            return VanillaResult::RelocateFailure;

        if (m_HookManager.InstallTrampolineHook(xorstr_("WKS::gc_heap::relocate_address"), relocateAddressAddress, reinterpret_cast<uintptr_t>(RelocateAddressHook), reinterpret_cast<uintptr_t*>(&oRelocateAddress)) != VanillaResult::Success)
            return VanillaResult::RelocateFailure;

        const uintptr_t entry2MethodDescAddress = m_PatternScanner.FindPatternInModule(xorstr_("55 8B EC 83 EC 0C 53 56 57 8B FA 8B D9 E8"), xorstr_("clr.dll"));
        if (!entry2MethodDescAddress)
            return VanillaResult::CLRStringFailure;

        entry2MethodDesc = reinterpret_cast<fnEntry2MethodDesc>(entry2MethodDescAddress);

        const uintptr_t getAddrOfSlotAddress = m_PatternScanner.FindPatternInModule(xorstr_("51 56 57 0F B7 41"), xorstr_("clr.dll"));
        if (!getAddrOfSlotAddress)
            return VanillaResult::CLRStringFailure;

        getAddrOfSlot = reinterpret_cast<fnGetAddrOfSlot>(getAddrOfSlotAddress);

        const uintptr_t allocateCLRStringAddress = m_PatternScanner.FindPatternInModule(xorstr_("53 8B D9 56 57 85 DB 0F"), xorstr_("clr.dll"));
        if (!allocateCLRStringAddress)
            return VanillaResult::CLRStringFailure;

        allocateCLRString = reinterpret_cast<fnAllocateCLRString>(allocateCLRStringAddress);

        setCLRStringAddress = m_PatternScanner.FindPatternInModule(xorstr_("89 02 81 F8"), xorstr_("clr.dll"));
        if (!setCLRStringAddress)
            return VanillaResult::CLRStringFailure;
    }

    return VanillaResult::Success;
}

HookManager& Vanilla::GetHookManager()
{
    return m_HookManager;
}

PatternScanner& Vanilla::GetPatternScanner()
{
    return m_PatternScanner;
}

void Vanilla::SetJITCallback(fnJITCallback callback)
{
    if (usingCLR)
        jitCallback = callback;
}

void Vanilla::RemoveJITCallback()
{
    if (usingCLR)
        jitCallback = nullptr;
}

void Vanilla::AddRelocation(std::reference_wrapper<std::uintptr_t> relocation)
{
    if (usingCLR)
        Relocations.push_back(relocation);
}

void Vanilla::RemoveRelocation(std::reference_wrapper<std::uintptr_t> relocation)
{
    if (!usingCLR)
        return;

    for (auto it = Relocations.begin(); it != Relocations.end(); ++it)
    {
        if (it->get() == relocation.get())
        {
            Relocations.erase(it);

            return;
        }
    }
}

void* Vanilla::GetMethodDesc(uintptr_t nativeCodeAddress)
{
    if (usingCLR)
        return entry2MethodDesc(reinterpret_cast<void*>(nativeCodeAddress), nullptr);

    return nullptr;
}

uintptr_t Vanilla::GetMethodSlotAddress(void* methodDesc)
{
    if (usingCLR)
        return getAddrOfSlot(methodDesc);

    return 0u;
}

[[clang::optnone]] CLRString* Vanilla::AllocateCLRString(const wchar_t* pwsz)
{
    if (usingCLR)
        return allocateCLRString(pwsz);

    return nullptr;
}

bool Vanilla::SetCLRString(uintptr_t address, CLRString* string)
{
    const uintptr_t clrStringCheckValue = *reinterpret_cast<uintptr_t*>(setCLRStringAddress + 0x4);
    const uint8_t clrStringShiftCount = *reinterpret_cast<uint8_t*>(setCLRStringAddress + 0xC);
    const int clrStringOffset = *reinterpret_cast<int*>(setCLRStringAddress + 0x10);

    *reinterpret_cast<CLRString**>(address) = string;

    if (reinterpret_cast<uintptr_t>(string) < clrStringCheckValue)
        return false;

    const uintptr_t flagAddress = (address >> clrStringShiftCount) + clrStringOffset;
    if (*reinterpret_cast<uint8_t*>(flagAddress) != 0xFF)
        *reinterpret_cast<uint8_t*>(flagAddress) = 0xFF;

    return true;
}
