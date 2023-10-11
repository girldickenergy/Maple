#pragma once

#include "../ISDK.h"
#include "../OsuMods.h"

class ModManager : public ISDK
{
    OsuMods* m_ModStatus = nullptr;

    void TryFindModStatus(uintptr_t start = 0u, unsigned int size = 0);

public:
    void __fastcall OnLoad(const std::shared_ptr<MapleBase>& mapleBase) override;
    void __fastcall OnJIT(uintptr_t address, unsigned size) override;
    std::string __fastcall GetName() override;

    OsuMods __fastcall GetActiveMods();
    bool __fastcall CheckActive(OsuMods mods);
    double __fastcall GetModPlaybackRate();
};
