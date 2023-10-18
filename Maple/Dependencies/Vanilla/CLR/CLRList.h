#pragma once

#include <algorithm>

#include "CLRArray.h"

template <typename T>
struct CLRList : CLRObject
{
    CLRArray<T>* Items;
    CLRObject* SyncRoot;
    size_t ListSize;
    size_t Version;

    size_t Size() const
    {
        return std::min(ListSize, Items->Size);
    }

    const T* Begin() const
    {
        return Items->Data();
    }

    const T* End() const
    {
        return Items->Data() + Size();
    }

    const T& Front() const
    {
        return *Begin();
    }

    const T& Back() const
    {
        return *(End() - 1);
    }

    const T& At(size_t index) const
    {
        return *(Begin() + index);
    }
};
