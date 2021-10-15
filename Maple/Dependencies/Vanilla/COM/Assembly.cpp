#include "Assembly.h"

Assembly::Assembly(mscorlib::_AssemblyPtr rawAssembly)
{
	Assembly::rawAssembly = rawAssembly;
}

std::string Assembly::GetFullNameUnsafe()
{
	BSTR bsAsmName;

	rawAssembly->get_FullName(&bsAsmName);

	std::wstring ws_asmName(bsAsmName, SysStringLen(bsAsmName));
	std::string s_asmName(ws_asmName.begin(), ws_asmName.end());

	return s_asmName;
}

Type Assembly::GetTypeUnsafe(std::string type)
{
	std::wstring w_type(type.begin(), type.end());
	BSTR b_type = SysAllocStringLen(w_type.data(), w_type.size());

	mscorlib::_TypePtr retType;
	rawAssembly->GetType_2(b_type, &retType);

	SysFreeString(b_type);

	return Type(retType);
}

std::vector<Type> Assembly::GetTypesUnsafe()
{
	SAFEARRAY* array;

	rawAssembly->GetTypes(&array);

	auto safeVector = SafeVector::SafeVector(array);

	auto retVector = std::vector<Type>();

	auto size = safeVector.count();

	for (int i = 0; i < size; i++)
	{
		mscorlib::_TypePtr rawType = NULL;

		safeVector.getElement(i, &rawType);

		auto type = Type(rawType);

		retVector.push_back(type);
	}

	return retVector;
}

Method Assembly::GetEntryPointUnsafe()
{
	mscorlib::_MethodInfoPtr method;
	rawAssembly->get_EntryPoint(&method);

	return Method(method);
}