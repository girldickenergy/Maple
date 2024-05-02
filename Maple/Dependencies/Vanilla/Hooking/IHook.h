#pragma once

#include "HookTypes.h"

class IHook
{
public:
    virtual HookTypes __fastcall GetType() = 0;
};