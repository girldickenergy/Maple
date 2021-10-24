#include "HitObjectManager.h"

#include <Vanilla.h>

#include "Player.h"

void HitObjectManager::Initialize()
{
	RawHitObjectManager = Vanilla::Explorer["osu.GameplayElements.HitObjectManager"];

	hitObjectManagerField = Player::RawPlayer["hitObjectManager"].Field;
	preEmptField = RawHitObjectManager["PreEmpt"].Field;
	preEmptSliderCompleteField = RawHitObjectManager["PreEmptSliderComplete"].Field;
	activeModsField = RawHitObjectManager["ActiveMods"].Field;
	hitWindow300Field = RawHitObjectManager["HitWindow300"].Field;
	hitWindow100Field = RawHitObjectManager["HitWindow100"].Field;
	hitWindow50Field = RawHitObjectManager["HitWindow50"].Field;
	hitObjectRadiusField = RawHitObjectManager["HitObjectRadius"].Field;

	obfuscatedType = activeModsField.GetTypeUnsafe();

	obfuscatedType["get_Value"].Method.Compile();
	obfuscatedGetValue = static_cast<fnObfuscatedGetValue>(obfuscatedType["get_Value"].Method.GetNativeStart());

	obfuscatedType["set_Value"].Method.Compile();
	obfuscatedSetValue = static_cast<fnObfuscatedSetValue>(obfuscatedType["set_Value"].Method.GetNativeStart());

	RawHitObjectManager["GetObject"].Method.Compile();
	getObject = static_cast<fnGetObject>(RawHitObjectManager["GetObject"].Method.GetNativeStart());
}

void* HitObjectManager::Instance()
{
	return *static_cast<void**>(hitObjectManagerField.GetAddress(Player::Instance()));
}

int HitObjectManager::GetPreEmpt()
{
	return *static_cast<int*>(preEmptField.GetAddress(Instance()));
}

void HitObjectManager::SetPreEmpt(int preEmpt)
{
	*static_cast<int*>(preEmptField.GetAddress(Instance())) = preEmpt;
}

int HitObjectManager::GetPreEmptSliderComplete()
{
	return *static_cast<int*>(preEmptSliderCompleteField.GetAddress(Instance()));
}

void HitObjectManager::SetPreEmptSliderComplete(int preEmptSliderComplete)
{
	*static_cast<int*>(preEmptSliderCompleteField.GetAddress(Instance())) = preEmptSliderComplete;
}

Mods HitObjectManager::GetActiveMods()
{
	void* activeModsInstance = *static_cast<void**>(activeModsField.GetAddress(Instance()));
	
	return obfuscatedGetValue(activeModsInstance);
}

void HitObjectManager::SetActiveMods(Mods mods)
{
	void* activeModsInstance = *static_cast<void**>(activeModsField.GetAddress(Instance()));

	obfuscatedSetValue(activeModsInstance, mods);
}

int HitObjectManager::GetHitWindow300()
{
	return *static_cast<int*>(hitWindow300Field.GetAddress(Instance()));
}

int HitObjectManager::GetHitWindow100()
{
	return *static_cast<int*>(hitWindow100Field.GetAddress(Instance()));
}

int HitObjectManager::GetHitWindow50()
{
	return *static_cast<int*>(hitWindow50Field.GetAddress(Instance()));
}

float HitObjectManager::GetHitObjectRadius()
{
	return *static_cast<float*>(hitObjectRadiusField.GetAddress(Instance()));
}

int HitObjectManager::GetCurrentHitObjectIndex()
{
	return *static_cast<int*>(currentHitObjectIndexField.GetAddress(Instance()));
}

int HitObjectManager::GetHitObjectsCount()
{
	return *static_cast<int*>(hitObjectsCountField.GetAddress(Instance()));
}

HitObject HitObjectManager::GetHitObject(int index)
{
	uintptr_t hitObjectInstance = getObject(index);

	HitObjectType type = *static_cast<HitObjectType*>(hitObjectInstace + )
}

double HitObjectManager::MapDifficultyRange(double difficulty, double min, double mid, double max, bool adjustToMods)
{
	if (adjustToMods)
		difficulty = ModManager::IsModEnabled(Mods::Easy) ? max(0.0, difficulty / 2.0) : ModManager::IsModEnabled(Mods::HardRock) ? min(10.0, difficulty * 1.4) : difficulty;

	if (difficulty > 5.)
		return mid + (max - mid) * (difficulty - 5.) / 5.;
	if (difficulty < 5.)
		return mid - (mid - min) * (5. - difficulty) / 5.;
	
	return mid;
}
