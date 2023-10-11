#pragma once

#include "ScoreInternal.h"
#include "../ISDK.h"

class Score : public ISDK
{
    ScoreInternal** m_Instance = nullptr;

    void __fastcall TryFindInstance(uintptr_t start = 0u, unsigned int size = 0);

    ScoreInternal* GetInstance();

public:
    void __fastcall OnLoad(const std::shared_ptr<MapleBase>& mapleBase) override;
    void __fastcall OnJIT(uintptr_t address, unsigned size) override;
    std::string __fastcall GetName() override;

    int __fastcall GetStartTime();
    void __fastcall SetStartTime(int startTime);
};