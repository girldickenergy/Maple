#include "Ruleset.h"

#include <Vanilla.h>

#include "Player.h"

void Ruleset::Initialize()
{
	RawRuleset = Vanilla::Explorer["osu.GameModes.Play.Rulesets.Ruleset"];
	RawRulesetFruits = Vanilla::Explorer["osu.GameModes.Play.Rulesets.Fruits.RulesetFruits"];
	rawSpriteManager = Vanilla::Explorer["osu.Graphics.Sprites.SpriteManager"];

	rulesetField = Player::RawPlayer["Ruleset"].Field;
	spriteManagerFlashlightField = RawRuleset["spriteManagerFlashlight"].Field;
	spriteManagerAlphaField = rawSpriteManager["Alpha"].Field;
	baseMovementSpeedField = RawRulesetFruits["baseMovementSpeed"].Field;
}

void* Ruleset::Instance()
{
	return *static_cast<void**>(rulesetField.GetAddress(Player::Instance()));
}

bool Ruleset::IsLoaded()
{
	return Player::IsLoaded() && Instance();
}

float Ruleset::GetFlashlightAlpha()
{
	void* spriteManagerInstance = *static_cast<void**>(spriteManagerFlashlightField.GetAddress(Instance()));
	if (!spriteManagerInstance)
		return 1.f;
	
	return *static_cast<float*>(spriteManagerAlphaField.GetAddress(spriteManagerInstance));
}

void Ruleset::SetFlashlightAlpha(float alpha)
{
	void* spriteManagerInstance = *static_cast<void**>(spriteManagerFlashlightField.GetAddress(Instance()));
	if (!spriteManagerInstance)
		return;

	*static_cast<float*>(spriteManagerAlphaField.GetAddress(spriteManagerInstance)) = alpha;
}

float Ruleset::GetCatcherSpeed()
{
	return *static_cast<float*>(baseMovementSpeedField.GetAddress(Instance()));
}

void Ruleset::SetCatcherSpeed(float speed)
{
	*static_cast<float*>(baseMovementSpeedField.GetAddress(Instance())) = speed;
}
