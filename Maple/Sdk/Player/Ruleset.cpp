#include "Ruleset.h"

#include <Vanilla.h>

#include "Player.h"

void Ruleset::Initialize()
{
	RawRuleset = Vanilla::Explorer["osu.GameModes.Play.Rulesets.Ruleset"];
	RawRulesetFruits = Vanilla::Explorer["osu.GameModes.Play.Rulesets.Fruits.RulesetFruits"];

	rulesetField = Player::RawPlayer["Ruleset"].Field;
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

float Ruleset::GetCatcherSpeed()
{
	void* instance = Instance();

	if (!instance)
		return 1.f;

	void* address = baseMovementSpeedField.GetAddress(instance);
	
	return *static_cast<float*>(address);
}

void Ruleset::SetCatcherSpeed(float speed)
{
	void* instance = Instance();

	if (!instance)
		return;

	void* address = baseMovementSpeedField.GetAddress(instance);

	if (!address)
		return;
	
	*static_cast<float*>(address) = speed;
}
