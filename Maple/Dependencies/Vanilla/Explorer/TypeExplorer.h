#pragma once

#include "MethodFieldExplorer.h"
#include "../COM/Type.h"

class TypeExplorer
{
public:
	Type Type;
	TypeExplorer(::Type type = ::Type());
	MethodFieldExplorer operator[] (std::string name)
	{
		auto method = FindMethod(name);
		auto field = FindField(name);

		return MethodFieldExplorer(method, field);
	}
	Method FindMethod(std::string method);
	Field FindField(std::string name);
};

