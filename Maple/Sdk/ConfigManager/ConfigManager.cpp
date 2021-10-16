#include "ConfigManager.h"

void ConfigManager::Initialize()
{
	RawConfigManager = Vanilla::Explorer["osu.Configuration.ConfigManager"];
	
	bindableBoolType = Vanilla::Explorer["osu.Helpers.BindableBool"];
	sCompatibilityContextAddress = RawConfigManager["sCompatibilityContext"].Field.GetAddress();
}

bool ConfigManager::CompatibilityContext()
{
	void* boolAddress = bindableBoolType["value"].Field.GetAddress(*static_cast<void**>(sCompatibilityContextAddress));

	return *static_cast<bool*>(boolAddress);
}
