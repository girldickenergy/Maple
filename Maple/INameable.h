#pragma once

#include <string>

class INameable
{
public:
    virtual std::string __fastcall GetName() = 0;
};
