#pragma once

#include <cstdint>

struct CLRObject
{
    uintptr_t VTable;
};

struct MarshalByRefObject : CLRObject
{
    uintptr_t Identity;
};