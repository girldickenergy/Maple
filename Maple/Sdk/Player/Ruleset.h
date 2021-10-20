#pragma once

#include <Explorer/TypeExplorer.h>

class Ruleset
{
	static inline TypeExplorer rawSpriteManager;

	static inline Field rulesetField;
	static inline Field spriteManagerFlashlightField;
	static inline Field spriteManagerAlphaField;
	static inline Field baseMovementSpeedField;
public:
	static inline TypeExplorer RawRuleset;
	static inline TypeExplorer RawRulesetFruits;
	
	static void Initialize();
	static void* Instance();
	static bool IsLoaded();
	static float GetFlashlightAlpha();
	static void SetFlashlightAlpha(float alpha);
	static float GetCatcherSpeed();
	static void SetCatcherSpeed(float speed);
};
