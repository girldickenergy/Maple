#pragma once

#include <string>
#include <vector>

#include "../mscorlib.h"

#include "Method.h"
#include "Type.h"

class Method;
class Type;

class Assembly
{
	mscorlib::_AssemblyPtr rawAssembly;
public:
	Assembly(mscorlib::_AssemblyPtr rawAssembly = nullptr);
	std::string GetFullNameUnsafe();
	Type GetTypeUnsafe(std::string type);
	std::vector<Type> GetTypesUnsafe();
	Method GetEntryPointUnsafe();
};
