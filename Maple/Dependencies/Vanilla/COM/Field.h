#pragma once

#include <string>

#include "../mscorlib.h"
#include "Type.h"

class Type;

class Field
{
	mscorlib::_FieldInfoPtr rawField;
	void* handle = nullptr;
	bool isStatic = false;
public:
	Field(mscorlib::_FieldInfoPtr rawField = nullptr);
	Type GetTypeUnsafe();
	void SetValueUnsafe(VARIANT instance, VARIANT value);
	variant_t GetValueUnsafe(variant_t instance);
	std::string GetFullNameUnsafe();
	int GetOffset();
	void* GetAddress(void* instance = nullptr);
};

