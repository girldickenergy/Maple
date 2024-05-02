#pragma once
#include "IHook.h"

#include <cstdint>
#include <vector>

class TrampolineHook : public IHook
{
    uintptr_t m_FunctionAddress;
    uintptr_t m_TrampolineAddress;
    std::vector<uint8_t> m_FunctionPrologue;

public:
    TrampolineHook(uintptr_t functionAddress, uintptr_t trampolineAddress, const std::vector<uint8_t>& functionPrologue)
    {
        m_FunctionAddress = functionAddress;
        m_TrampolineAddress = trampolineAddress;
        m_FunctionPrologue = functionPrologue;
    }

    uintptr_t __fastcall GetFunctionAddress() const
    {
        return m_FunctionAddress;
    }

    uintptr_t __fastcall GetTrampolineAddress() const
    {
        return m_TrampolineAddress;
    }

    const std::vector<uint8_t>& __fastcall GetFunctionPrologue()
    {
        return m_FunctionPrologue;
    }

    HookTypes __fastcall GetType() override
    {
        return HookTypes::Trampoline;
    }
};