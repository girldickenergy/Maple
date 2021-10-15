#include "TypeExplorer.h"

#include "../NameCache.h"

TypeExplorer::TypeExplorer(::Type type)
{
	Type = type;
}

Method TypeExplorer::FindMethod(std::string method)
{
	return Type.GetMethodUnsafe(NameCache::GetObfuscatedName(method));
}

Field TypeExplorer::FindField(std::string field)
{
	return Type.GetFieldUnsafe(NameCache::GetObfuscatedName(field));
}