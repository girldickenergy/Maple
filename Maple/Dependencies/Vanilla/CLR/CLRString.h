#pragma once

#include <string>

class CLRString
{
    struct StringObject
    {
        void* vtable;
        int length;
        wchar_t* buffer;
    };

    StringObject ptr;

public:
    int Length() const
    {
        return ptr.length;
    }

    std::wstring_view Data() const
    {
        return reinterpret_cast<const wchar_t*>(&ptr.buffer);
    }
};