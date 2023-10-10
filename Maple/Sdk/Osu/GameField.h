#pragma once

#include "../ISDK.h"
#include "GameFieldInternal.h"

class GameField : public ISDK
{
    uintptr_t m_InternalInstanceAddress = 0u;

    void __fastcall TryFindInternalInstance(uintptr_t start = 0u, unsigned int size = 0);

public:
    void __fastcall OnLoad(const std::shared_ptr<MapleBase>& mapleBase) override;
    void __fastcall OnJIT(uintptr_t address, unsigned size) override;
    std::string __fastcall GetName() override;

    GameFieldInternal* GetInternalInstance();

    float __fastcall GetRatio();
    Vector2 __fastcall DisplayToField(Vector2 display);
    Vector2 __fastcall FieldToDisplay(Vector2 field);
};