#pragma once
#include "IHook.h"

#include <cstdint>

class PointerRedirectionHook : public IHook
{
    uintptr_t m_PointerAddress;
    uintptr_t m_OriginalFunctionAddress;

public:
    PointerRedirectionHook(uintptr_t pointerAddress, uintptr_t originalFunctionAddress)
    {
        m_PointerAddress = pointerAddress;
        m_OriginalFunctionAddress = originalFunctionAddress;
    }

    uintptr_t __fastcall GetPointerAddress() const
    {
        return m_PointerAddress;
    }

    uintptr_t __fastcall GetOriginalFunctionAddress() const
    {
        return m_OriginalFunctionAddress;
    }

    HookTypes __fastcall GetType() override
    {
        return HookTypes::PointerRedirection;
    }
};