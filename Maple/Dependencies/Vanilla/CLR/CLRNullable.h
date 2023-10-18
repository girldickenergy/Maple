#pragma once

#include "CLRObject.h"

template <typename T>
struct Nullable : CLRObject
{
    T Value;
};