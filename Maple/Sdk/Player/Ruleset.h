#pragma once

#include <Explorer/TypeExplorer.h>

class Ruleset
{
	static inline Field rulesetField;
	static inline Field baseMovementSpeedField;
public:
	static inline TypeExplorer RawRuleset;
	static inline TypeExplorer RawRulesetFruits;
	
	static void Initialize();
	static void* Instance();
	static bool IsLoaded();
	static float GetCatcherSpeed();
	static void SetCatcherSpeed(float speed);
};
