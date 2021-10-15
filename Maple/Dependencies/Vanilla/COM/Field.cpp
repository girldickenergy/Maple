#include "Field.h"

#include "Helpers/FieldHelpers.h"

Field::Field(mscorlib::_FieldInfoPtr rawField)
{
	Field::rawField = rawField;

	if (rawField)
	{
		mscorlib::RuntimeFieldHandle boxedHandle;
		rawField->get_FieldHandle(&boxedHandle);
		handle = boxedHandle.m_ptr;

		VARIANT_BOOL isStaticBoxed;
		rawField->get_IsStatic(&isStaticBoxed);
		isStatic = isStaticBoxed == -1;
	}
}

Type Field::GetTypeUnsafe()
{
	mscorlib::_TypePtr type;
	rawField->get_FieldType(&type);

	return Type(type);
}

variant_t Field::GetValueUnsafe(variant_t instance)
{
	variant_t retVal;
	rawField->GetValue(instance, &retVal);

	return retVal;
}

void Field::SetValueUnsafe(VARIANT instance, VARIANT value)
{
	rawField->SetValue_2(instance, value);
}

std::string Field::GetFullNameUnsafe()
{
	BSTR bs_name;

	rawField->get_name(&bs_name);

	std::wstring ws_name(bs_name, SysStringLen(bs_name));
	std::string s_name(ws_name.begin(), ws_name.end());

	return s_name;
}

int Field::GetOffset()
{
	return (*reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(handle) + 0x8) & 0xFFFFFF) + (isStatic ? 0x0 : 0x4); //skip vtable if member field
}

void* Field::GetAddress(void* instance)
{
	if (instance == nullptr && isStatic)
		return FieldHelpers::GetStaticFieldAddress(handle);
	
	if (instance != nullptr)
		return reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(instance) + GetOffset());

	return nullptr;
}
