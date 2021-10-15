#pragma once

#include <string>
#include <vector>

#include "../mscorlib.h"

#include "Assembly.h"
#include "Field.h"
#include "Method.h"

class Assembly;
class Field;
class Method;

class Type
{   
public:
    mscorlib::_TypePtr RawType;
    Type(mscorlib::_TypePtr rawType = nullptr);
    std::string GetFullNameUnsafe();
    Type GetTypeUnsafe();
    Method GetMethodUnsafe(std::string name, bool emptyTypes);
    Method GetMethodUnsafe(const std::string& name);
    Method GetMethodUnsafe(std::string name, SAFEARRAY* types);
    Method GetMethodUnsafe(std::string name, std::vector<Type> types);
    Field GetFieldUnsafe(const std::string& name);
    Assembly GetAssemblyUnsafe();
    std::vector<Method> GetMethodsUnsafe();
    std::vector<Field> GetFieldsUnsafe();
};

