#include "Type.h"

Type::Type(mscorlib::_TypePtr rawType) : RawType(rawType) {}

std::string Type::GetFullNameUnsafe()
{
    BSTR bs_name;

    RawType->get_FullName(&bs_name);

    std::wstring ws_name(bs_name, SysStringLen(bs_name));
    std::string s_name(ws_name.begin(), ws_name.end());

    return s_name;
}

Method Type::GetMethodUnsafe(const std::string& name)
{
    std::wstring w_name(name.begin(), name.end());
    BSTR b_name = SysAllocStringLen(w_name.data(), w_name.size());

    mscorlib::_MethodInfoPtr method;

    RawType->GetMethod_2(b_name, static_cast<mscorlib::BindingFlags>(mscorlib::BindingFlags_Instance |
        mscorlib::BindingFlags_Public | mscorlib::BindingFlags_NonPublic |
        mscorlib::BindingFlags_Static), &method);

    SysFreeString(b_name);

    return Method(method);
}

Type Type::GetTypeUnsafe()
{
    mscorlib::_TypePtr type;
    RawType->GetType(&type);

    return Type(type);
}

Method Type::GetMethodUnsafe(std::string name, bool emptyTypes)
{
    if (emptyTypes) 
    {
        auto type = GetTypeUnsafe();

        auto empty = type.GetAssemblyUnsafe().
            GetTypeUnsafe("System.Type").
            GetFieldUnsafe("EmptyTypes").
            GetValueUnsafe(variant_t());

        return GetMethodUnsafe(name, empty.parray);
    }
	
    return GetMethodUnsafe(name);
}

Method Type::GetMethodUnsafe(std::string name, std::vector<Type> types)
{
    std::wstring w_name(name.begin(), name.end());
    BSTR b_name = SysAllocStringLen(w_name.data(), w_name.size());

    mscorlib::_MethodInfoPtr method;
    SAFEARRAY* psa;
    SAFEARRAYBOUND rgsabound[1];

    rgsabound[0].lLbound = 0;
    rgsabound[0].cElements = types.size();

    psa = SafeArrayCreate(VT_UNKNOWN, 1, rgsabound);

    auto safeVector = SafeVector(psa);

    for (int i = 0; i < types.size(); i++)
    {
        auto var = static_cast<IUnknown*>(types.at(i).RawType);
        safeVector.setElement(i, var);
    }

    auto meth = GetMethodUnsafe(name, safeVector.asSafeArray());

    SafeArrayDestroy(safeVector.asSafeArray());

    SysFreeString(b_name);

    return meth;
}

Method Type::GetMethodUnsafe(std::string name, SAFEARRAY* types)
{
    std::wstring w_name(name.begin(), name.end());
    BSTR b_name = SysAllocStringLen(w_name.data(), w_name.size());

    mscorlib::_MethodInfoPtr method;

    RawType->GetMethod(b_name, static_cast<mscorlib::BindingFlags>(mscorlib::BindingFlags_Instance |
        mscorlib::BindingFlags_Public | mscorlib::BindingFlags_NonPublic |
        mscorlib::BindingFlags_Static), nullptr, types, nullptr, &method);

    SysFreeString(b_name);

    return Method(method);
}

Field Type::GetFieldUnsafe(const std::string& name)
{
    std::wstring w_name(name.begin(), name.end());
    BSTR b_name = SysAllocStringLen(w_name.data(), w_name.size());

    mscorlib::_FieldInfoPtr field;

    RawType->GetField(b_name, static_cast<mscorlib::BindingFlags>(mscorlib::BindingFlags_Instance |
        mscorlib::BindingFlags_Public | mscorlib::BindingFlags_NonPublic |
        mscorlib::BindingFlags_Static), &field);

    SysFreeString(b_name);

    return Field(field);
}

Assembly Type::GetAssemblyUnsafe()
{
    mscorlib::_AssemblyPtr assembly;

    RawType->get_Assembly(&assembly);

    return Assembly(assembly);
}

std::vector<Method> Type::GetMethodsUnsafe()
{
    SAFEARRAY* array;

    RawType->GetMethods(static_cast<mscorlib::BindingFlags>(
        mscorlib::BindingFlags_Instance |
        mscorlib::BindingFlags_Public | mscorlib::BindingFlags_NonPublic |
        mscorlib::BindingFlags_Static), &array);

    auto safeVector = SafeVector(array);
    auto retVector = std::vector<Method>();

    auto size = safeVector.count();
    for (int i = 0; i < size; i++) 
    {
        mscorlib::_MethodInfoPtr rawMethod = NULL;

        safeVector.getElement(i, &rawMethod);

        auto method = Method(rawMethod);

        retVector.push_back(method);
    }

    return retVector;
}

std::vector<Field> Type::GetFieldsUnsafe()
{
    SAFEARRAY* array;

    RawType->GetFields(static_cast<mscorlib::BindingFlags>(
        mscorlib::BindingFlags_Instance |
        mscorlib::BindingFlags_Public | mscorlib::BindingFlags_NonPublic |
        mscorlib::BindingFlags_Static), &array);

    auto safeVector = SafeVector(array);
    auto retVector = std::vector<Field>();

    auto size = safeVector.count();
    for (int i = 0; i < size; i++)
    {
        mscorlib::_FieldInfoPtr rawField = NULL;

        safeVector.getElement(i, &rawField);

        auto field = Field(rawField);

        retVector.push_back(field);
    }

    return retVector;
}