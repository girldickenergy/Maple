#pragma once

#include <Vanilla.h>

class ConfigManager
{
	static inline TypeExplorer bindableBoolType;
	static inline void* sCompatibilityContextAddress = nullptr;
public:
	static inline TypeExplorer RawConfigManager;
	
	static void Initialize();
	static bool CompatibilityContext();
};
