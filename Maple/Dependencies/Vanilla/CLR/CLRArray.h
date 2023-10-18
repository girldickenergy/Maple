#pragma once

#include "CLRObject.h"

template <typename T>
struct CLRArray : CLRObject
{
    size_t Size;
    T First;

    T* Data()
    {
        return &First;
    }
};
