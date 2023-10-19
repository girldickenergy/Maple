#pragma once

#include "PlayModes.h"
#include "PlayerInternal.h"
#include "../../ISDK.h"

class Player : ISDK
{
    PlayerInternal** m_Instance = nullptr;
    bool* m_IsRetrying = nullptr;
    bool* m_IsFailed = nullptr;

    void __fastcall TryFindInstance(uintptr_t start = 0u, unsigned int size = 0);
    void __fastcall TryFindIsRetrying(uintptr_t start = 0u, unsigned int size = 0);
    void __fastcall TryFindIsFailed(uintptr_t start = 0u, unsigned int size = 0);

    PlayerInternal* GetInstance();

public:
    void __fastcall OnLoad(const std::shared_ptr<MapleBase>& mapleBase) override;
    void __fastcall OnJIT(uintptr_t address, unsigned size) override;
    std::string __fastcall GetName() override;

    bool GetIsLoaded();
    bool GetIsReplayMode();
    PlayModes GetPlayMode();
    bool GetIsRetrying();
    bool GetIsFailed();
};
