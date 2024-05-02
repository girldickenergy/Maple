#pragma once

#include "../ISDK.h"

class AudioEngine : public ISDK
{
    int* m_Time = nullptr;

    void TryFindTime(uintptr_t start = 0u, unsigned int size = 0);

public:
    void __fastcall OnLoad(const std::shared_ptr<MapleBase>& mapleBase) override;
    const char* __fastcall GetName() override;

    int __fastcall GetTime();
    bool __fastcall GetIsPaused();
};