#pragma once

#include "../ISDK.h"
#include "GameFieldInternal.h"

class GameField : public ISDK
{
    GameFieldInternal** m_Instance = nullptr;

    void __fastcall TryFindInstance(uintptr_t start = 0u, unsigned int size = 0);

    GameFieldInternal* GetInstance();

public:
    void __fastcall OnLoad(const std::shared_ptr<MapleBase>& mapleBase) override;
    const char* __fastcall GetName() override;

    Vector2 GetSize();
    float __fastcall GetRatio();
    Vector2 __fastcall GetOffset();
    Vector2 __fastcall DisplayToField(Vector2 display);
    Vector2 __fastcall FieldToDisplay(Vector2 field);
};