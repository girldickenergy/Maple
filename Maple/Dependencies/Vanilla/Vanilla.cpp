#include "Vanilla.h"

#if __has_include("xorstr.h")
    #include "xorstr.h"
#else
    #define xorstr_(string) string
#endif

int __stdcall Vanilla::CompileMethodHook(uintptr_t instance, uintptr_t compHnd, uintptr_t methodInfo, unsigned int flags, uintptr_t* entryAddress, unsigned int* nativeSizeOfCode)
{
    const int ret = oCompileMethod(instance, compHnd, methodInfo, flags, entryAddress, nativeSizeOfCode);

    if (ret == 0 && jitCallback)
        jitCallback(*entryAddress, *nativeSizeOfCode);

    return ret;
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
        const uintptr_t compileMethodAddress = m_PatternScanner.FindPatternInModule(xorstr_("55 8B EC 83 E4 F8 83 EC 1C 53 8B 5D 10"), xorstr_("clrjit.dll"));
        if (!compileMethodAddress)
            return VanillaResult::JITFailure;

        if (m_HookManager.InstallHook(xorstr_("JITHook"), compileMethodAddress, reinterpret_cast<uintptr_t>(CompileMethodHook), reinterpret_cast<uintptr_t*>(&oCompileMethod)) != VanillaResult::Success)
            return VanillaResult::JITFailure;

        const uintptr_t relocateAddressAddress = m_PatternScanner.FindPatternInModule(xorstr_("55 8B EC 57 8B 7D 08 8B 0F 3B 0D"), xorstr_("clr.dll"));
        if (!relocateAddressAddress)
            return VanillaResult::RelocateFailure;

        if (m_HookManager.InstallHook(xorstr_("RelocateAddressHook"), relocateAddressAddress, reinterpret_cast<uintptr_t>(RelocateAddressHook), reinterpret_cast<uintptr_t*>(&oRelocateAddress)) != VanillaResult::Success)
            return VanillaResult::RelocateFailure;

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
