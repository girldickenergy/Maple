#pragma once

#include "TypeExplorer.h"
#include "../COM/Assembly.h"

class Explorer
{
public:
	Assembly Assembly;
	Explorer();
	Explorer(::Assembly assembly);
	TypeExplorer operator[] (std::string name)
	{
		return FindType(name);
	}
	TypeExplorer FindType(std::string type);
};
