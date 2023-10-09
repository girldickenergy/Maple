#pragma once

#include "../MapleBase.h"

class ISDK
{
protected:
    std::shared_ptr<MapleBase> m_MapleBase;

public:
    virtual void __fastcall OnLoad(const std::shared_ptr<MapleBase>& mapleBase)
    {
        m_MapleBase = mapleBase;
    }

    /**
     * \brief Executed whenever new function gets compiled
     */
    virtual void __fastcall OnJIT(uintptr_t address, unsigned int size) {}

    /**
     * \returns This SDK's name
     */
    virtual std::string GetName() = 0;
};
