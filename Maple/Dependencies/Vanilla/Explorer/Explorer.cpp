#include "Explorer.h"

#include "../NameCache.h"

Explorer::Explorer()
{
}

Explorer::Explorer(::Assembly assembly)
{
	Assembly = assembly;
}

TypeExplorer Explorer::FindType(std::string type)
{
	return Assembly.GetTypeUnsafe(NameCache::GetObfuscatedName(type));
}
