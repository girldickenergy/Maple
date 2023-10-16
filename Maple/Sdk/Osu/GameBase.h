#pragma once

#include "Numerics/RectangleI.h"

#include "../ISDK.h"
#include "../OsuModes.h"

class GameBase : public ISDK
{
    int* m_Time = nullptr;
    OsuModes* m_Mode = nullptr;
    bool* m_IsFullscreen = nullptr;
    uintptr_t m_ClientBoundsAddress = 0u;
    bool m_FoundTickrate = false;
    static inline double m_Tickrate = 1000.0 / 60.0;

    void TryFindTime(uintptr_t start = 0u, unsigned int size = 0u);
    void TryFindMode(uintptr_t start = 0u, unsigned int size = 0u);
    void TryFindIsFullscreen(uintptr_t start = 0u, unsigned int size = 0u);
    void TryFindClientBounds(uintptr_t start = 0u, unsigned int size = 0u);
    void TryPatchTickrate(uintptr_t start = 0u, unsigned int size = 0u);

public:
    void __fastcall OnLoad(const std::shared_ptr<MapleBase>& mapleBase) override;
    void __fastcall OnJIT(uintptr_t address, unsigned size) override;
    std::string __fastcall GetName() override;

    int GetTime();
    OsuModes GetMode();
    bool GetIsFullscreen();
    RectangleI GetClientBounds();
    void SetTickrate(double value);
};