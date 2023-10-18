#pragma once

#include "CLRObject.h"

template <typename T>
struct CLRNullable : CLRObject
{
    T Value;
};