#pragma once

#include "Mods.h"

#include "../ISDK.h"

class ModManager : public ISDK
{
    Mods* m_ModStatus = nullptr;

    void TryFindModStatus(uintptr_t start = 0u, unsigned int size = 0);

public:
    void __fastcall OnLoad(const std::shared_ptr<MapleBase>& mapleBase) override;
    const char* __fastcall GetName() override;

    Mods __fastcall GetActiveMods();
    bool __fastcall CheckActive(Mods mods);
    double __fastcall GetModPlaybackRate();
};
