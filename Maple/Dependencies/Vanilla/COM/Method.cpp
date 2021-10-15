#include "Method.h"

#include "Helpers/JItHelpers.h"

Method::Method(mscorlib::_MethodInfoPtr rawMethod) : rawMethod(rawMethod) {}

std::string Method::GetFullNameUnsafe()
{
    BSTR bs_Name;

    rawMethod->get_name(&bs_Name);

    std::wstring ws_Name(bs_Name, SysStringLen(bs_Name));
    std::string s_Name(ws_Name.begin(), ws_Name.end());

    return s_Name;
}

Type Method::GetTypeUnsafe()
{
    mscorlib::_TypePtr t;

    rawMethod->GetType(&t);

    return Type(t);
}

variant_t Method::InvokeUnsafe(variant_t obj, std::vector<variant_t>& args)
{
    SAFEARRAY* psa;
    SAFEARRAYBOUND rgsabound[1];

    rgsabound[0].lLbound = 0;
    rgsabound[0].cElements = (ULONG)args.size();
    psa = SafeArrayCreate(VT_VARIANT, 1, rgsabound);

    for (LONG i = 0; i < (LONG)args.size(); i++)
        SafeArrayPutElement(psa, &i, &args[i]);

    variant_t ret = Method::InvokeUnsafe(obj, psa);

    SafeArrayDestroy(psa);

    return ret;
}

VARIANT Method::InvokeUnsafe(VARIANT instance, SAFEARRAY* parameters)
{
    VARIANT retVal;

    rawMethod->Invoke_3(instance, parameters, &retVal);
    return retVal;
}

void* Method::GetNativeStart()
{
    auto v = new VARIANTARG();

    v->punkVal = rawMethod;
    v->vt = VARENUM::VT_UNKNOWN;
    auto instance = new VARIANT(*v);

    auto rtMethodInfo = GetTypeUnsafe();
    
    auto args = std::vector<variant_t>();
    auto rtMethodHandle = rtMethodInfo.
        GetMethodUnsafe("get_MethodHandle").
        InvokeUnsafe(*instance, args);

    void* pMethodDesc = rtMethodInfo.GetAssemblyUnsafe().GetTypeUnsafe("System.RuntimeMethodHandle").GetMethodUnsafe("get_Value").InvokeUnsafe(rtMethodHandle, nullptr).byref;
    void* addrFromClr = JitHelpers::GetFunctionPointer(pMethodDesc);
    if (addrFromClr != nullptr)
        return addrFromClr;

    auto getFunctionPointer = rtMethodInfo.GetAssemblyUnsafe().
        GetTypeUnsafe("System.RuntimeMethodHandle").
        GetMethodUnsafe("GetFunctionPointer", true);

    auto pointer = getFunctionPointer.InvokeUnsafe(rtMethodHandle, args);

    return pointer.byref;
}

void Method::Compile()
{
    auto v = new VARIANTARG();

    v->punkVal = rawMethod;
    v->vt = VARENUM::VT_UNKNOWN;
    auto instance = new VARIANT(*v);

    auto rtMethodInfo = GetTypeUnsafe();
	
    auto args = std::vector<variant_t>();
    auto rtMethodHandle = rtMethodInfo.
        GetMethodUnsafe("get_MethodHandle").
        InvokeUnsafe(*instance, args);

    auto types = std::vector<Type>();
    types.push_back(rtMethodInfo.GetAssemblyUnsafe().GetTypeUnsafe("System.RuntimeMethodHandle"));
    Method prepareMethod = rtMethodInfo.GetAssemblyUnsafe().GetTypeUnsafe("System.Runtime.CompilerServices.RuntimeHelpers").GetMethodUnsafe("PrepareMethod", types);

	args.push_back(rtMethodHandle);
    prepareMethod.InvokeUnsafe(variant_t(), args);

    void* pMethodDesc = rtMethodInfo.GetAssemblyUnsafe().GetTypeUnsafe("System.RuntimeMethodHandle").GetMethodUnsafe("get_Value").InvokeUnsafe(rtMethodHandle, nullptr).byref;
    JitHelpers::CompilePrecode(GetNativeStart(), pMethodDesc);
}
