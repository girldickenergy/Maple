#pragma once

#include "../COM/Method.h"

class MethodFieldExplorer
{
public:
	Method Method;
	Field Field;
	MethodFieldExplorer(::Method method, ::Field field);
};

