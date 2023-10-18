#pragma once

#include <string>

#include "CLRObject.h"

struct CLRString : CLRObject
{
    size_t Length;
    wchar_t* Text;

    std::wstring_view Data() const
    {
        return reinterpret_cast<const wchar_t*>(&Text);
    }
};