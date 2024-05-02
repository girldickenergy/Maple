#pragma once

#include <string>

class INameable
{
public:
    virtual const char* __fastcall GetName() = 0;
};
