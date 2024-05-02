#pragma once

#include "MapleBase.h"

class IMapleComponent
{
protected:
    static inline std::shared_ptr<MapleBase> m_MapleBase = nullptr;

public:
    virtual void __fastcall OnLoad(const std::shared_ptr<MapleBase>& mapleBase)
    {
        m_MapleBase = mapleBase;
    }

    /**
     * \brief Executed whenever new function gets compiled
     */
    virtual void __fastcall OnJIT(void* methodDesc, uintptr_t functionAddress, size_t functionSize) {}
};