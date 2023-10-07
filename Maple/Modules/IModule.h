#pragma once

#include "MapleBase.h"

class IModule
{
protected:
    std::shared_ptr<MapleBase> m_MapleBase;

public:
    virtual void __fastcall OnLoad(const std::shared_ptr<MapleBase>& mapleBase)
    {
        m_MapleBase = mapleBase;
    }

    virtual void __fastcall OnUpdate() = 0;
    virtual void __fastcall OnRender() = 0;
};
