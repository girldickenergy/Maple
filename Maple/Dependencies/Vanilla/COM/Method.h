#pragma once

#include <string>
#include <vector>

#include "../mscorlib.h"

#include "SafeVector.h"
#include "Type.h"

class Type;

class Method
{
    mscorlib::_MethodInfoPtr rawMethod;
public:
    Method(mscorlib::_MethodInfoPtr rawMethod = nullptr);
    std::string GetFullNameUnsafe();
    Type GetTypeUnsafe();
    variant_t InvokeUnsafe(variant_t obj, std::vector<variant_t>& args);
    VARIANT InvokeUnsafe(VARIANT instance, SAFEARRAY* parameters);
    void* GetNativeStart();
    void Compile();
};

