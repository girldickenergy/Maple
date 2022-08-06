#include "HitObjectManager.h"

#include "Player.h"
#include "../Helpers/Obfuscated.h"
#include "Math/Vector2.h"
#include "Osu/HitObjects/HitObjectType.h"
#include "../Graphics/SpriteManager.h"

void HitObjectManager::CacheHitObjects()
{
	hitObjects.clear();
	
	auto isAddressValid = [](uintptr_t address)
	{
		return address && *reinterpret_cast<int*>(address);
	};

	auto getHitObjectListItemsAddress = []()
	{
		const uintptr_t instance = GetInstance();

		return instance ? *reinterpret_cast<uintptr_t*>(*reinterpret_cast<uintptr_t*>(instance + HITOBJECTMANAGER_HITOBJECTS_OFFSET) + 0x4) : 0u;
	};

	const int hitObjectsCount = GetHitObjectsCount();
	for (int i = 0; i < hitObjectsCount; i++)
	{
		const uintptr_t hitObjectListItemsAddress = getHitObjectListItemsAddress();
		if (!isAddressValid(hitObjectListItemsAddress))
		{
			i--;

			continue;
		}

		const uintptr_t hitObjectAddress = *reinterpret_cast<uintptr_t*>(hitObjectListItemsAddress + 0x8 + 0x4 * i);
		if (!isAddressValid(hitObjectAddress))
		{
			i--;

			continue;
		}

		const HitObjectType type = *reinterpret_cast<HitObjectType*>(hitObjectAddress + HITOBJECT_TYPE_OFFSET);
		const int startTime = *reinterpret_cast<int*>(hitObjectAddress + HITOBJECT_STARTTIME_OFFSET);
		const int endTime = *reinterpret_cast<int*>(hitObjectAddress + HITOBJECT_ENDTIME_OFFSET);
		const Vector2 position = *reinterpret_cast<Vector2*>(hitObjectAddress + HITOBJECT_POSITION_OFFSET);
		const int segmentCount = *reinterpret_cast<int*>(hitObjectAddress + HITOBJECT_SEGMENTCOUNT_OFFSET);
		const double spatialLength = *reinterpret_cast<double*>(hitObjectAddress + HITOBJECT_SPATIALLENGTH_OFFSET);

		if ((type & HitObjectType::Slider) > HitObjectType::None)
		{
			const Vector2 endPosition = *reinterpret_cast<Vector2*>(hitObjectAddress + SLIDEROSU_ENDPOSITION_OFFSET);

			std::vector<Vector2> sliderCurvePoints;
			std::vector<std::pair<Vector2, Vector2>> sliderCurveSmoothLines;
			std::vector<double> cumulativeLengths;

			const uintptr_t sliderCurvePointsAddress = *reinterpret_cast<uintptr_t*>(hitObjectAddress + SLIDEROSU_SLIDERCURVEPOINTS_OFFSET);
			if (!isAddressValid(sliderCurvePointsAddress))
			{
				i--;

				continue;
			}

			const uintptr_t sliderCurvePointsItemsAddress = *reinterpret_cast<uintptr_t*>(sliderCurvePointsAddress + 0x04);
			if (!isAddressValid(sliderCurvePointsItemsAddress))
			{
				i--;

				continue;
			}

			const int sliderCurvePointsCount = *reinterpret_cast<int*>(sliderCurvePointsAddress + 0x0C);

			for (int j = 0; j < sliderCurvePointsCount; j++)
			{
				Vector2 point = *reinterpret_cast<Vector2*>(sliderCurvePointsItemsAddress + 0x08 + 0x08 * j);

				sliderCurvePoints.emplace_back(point);
			}

			if (Player::GetPlayMode() != PlayModes::Taiko)
			{
				const uintptr_t sliderCurveSmoothLinesAddress = *reinterpret_cast<uintptr_t*>(hitObjectAddress + SLIDEROSU_SLIDERCURVESMOOTHLINES_OFFSET);
				if (!isAddressValid(sliderCurveSmoothLinesAddress))
				{
					i--;

					continue;
				}

				const uintptr_t sliderCurveSmoothLinesItemsAddress = *reinterpret_cast<uintptr_t*>(sliderCurveSmoothLinesAddress + 0x04);
				if (!isAddressValid(sliderCurveSmoothLinesItemsAddress))
				{
					i--;

					continue;
				}

				const int sliderCurveSmoothLinesCount = *reinterpret_cast<int*>(sliderCurveSmoothLinesAddress + 0x0C);

				for (int j = 0; j < sliderCurveSmoothLinesCount; j++)
				{
					const uintptr_t item = *reinterpret_cast<uintptr_t*>(sliderCurveSmoothLinesItemsAddress + 0x08 + 0x04 * j);

					Vector2 point1 = *reinterpret_cast<Vector2*>(item + 0x08);
					Vector2 point2 = *reinterpret_cast<Vector2*>(item + 0x10);

					sliderCurveSmoothLines.emplace_back(point1, point2);
				}

				const uintptr_t cumulativeLengthsAddress = *reinterpret_cast<uintptr_t*>(hitObjectAddress + SLIDEROSU_CUMULATIVELENGTHS_OFFSET);
				if (!isAddressValid(cumulativeLengthsAddress))
				{
					i--;

					continue;
				}

				const uintptr_t cumulativeLengthsItemsAddress = *reinterpret_cast<uintptr_t*>(cumulativeLengthsAddress + 0x4);
				if (!isAddressValid(cumulativeLengthsItemsAddress))
				{
					i--;

					continue;
				}

				const int cumulativeLengthsCount = *reinterpret_cast<int*>(cumulativeLengthsAddress + 0xC);

				for (int j = 0; j < cumulativeLengthsCount; j++)
					cumulativeLengths.emplace_back(*reinterpret_cast<double*>(cumulativeLengthsItemsAddress + 0x8 + 0x8 * j));
			}

			hitObjects.emplace_back(type, startTime, endTime, position, endPosition, segmentCount, spatialLength, sliderCurvePoints, sliderCurveSmoothLines, cumulativeLengths);
		}
		else hitObjects.emplace_back(type, startTime, endTime, position, position, segmentCount, spatialLength);
	}
}

HitObject HitObjectManager::GetHitObject(int index)
{
	return hitObjects[index];
}

uintptr_t HitObjectManager::GetInstance()
{
	const uintptr_t playerInstance = Player::GetInstance();

	return playerInstance ? *reinterpret_cast<uintptr_t*>(playerInstance + HITOBJECTMANAGER_INSTANCE_OFFSET) : 0u;
}

int HitObjectManager::GetPreEmpt()
{
	const uintptr_t hitObjectManagerInstance = GetInstance();

	return hitObjectManagerInstance ? *reinterpret_cast<int*>(hitObjectManagerInstance + HITOBJECTMANAGER_PREEMPT_OFFSET) : 0;
}

void HitObjectManager::SetPreEmpt(int value)
{
	if (const uintptr_t hitObjectManagerInstance = GetInstance())
		*reinterpret_cast<int*>(hitObjectManagerInstance + HITOBJECTMANAGER_PREEMPT_OFFSET) = value;
}

int HitObjectManager::GetPreEmptSliderComplete()
{
	const uintptr_t hitObjectManagerInstance = GetInstance();

	return hitObjectManagerInstance ? *reinterpret_cast<int*>(hitObjectManagerInstance + HITOBJECTMANAGER_PREEMPTSLIDERCOMPLETE_OFFSET) : 0;
}

void HitObjectManager::SetPreEmptSliderComplete(int value)
{
	if (const uintptr_t hitObjectManagerInstance = GetInstance())
		*reinterpret_cast<int*>(hitObjectManagerInstance + HITOBJECTMANAGER_PREEMPTSLIDERCOMPLETE_OFFSET) = value;
}

Mods HitObjectManager::GetActiveMods()
{
	const uintptr_t hitObjectManagerInstance = GetInstance();

	return hitObjectManagerInstance ? static_cast<Mods>(Obfuscated::GetInt(*reinterpret_cast<uintptr_t*>(hitObjectManagerInstance + HITOBJECTMANAGER_ACTIVEMODS_OFFSET))) : Mods::None;
}

void HitObjectManager::SetActiveMods(Mods value)
{
	if (const uintptr_t hitObjectManagerInstance = GetInstance())
		Obfuscated::SetInt(*reinterpret_cast<uintptr_t*>(hitObjectManagerInstance + HITOBJECTMANAGER_ACTIVEMODS_OFFSET), static_cast<int>(value));
}

int HitObjectManager::GetHitWindow50()
{
	const uintptr_t hitObjectManagerInstance = GetInstance();

	return hitObjectManagerInstance ? *reinterpret_cast<int*>(hitObjectManagerInstance + HITOBJECTMANAGER_HITWINDOW50_OFFSET) : 0;
}

int HitObjectManager::GetHitWindow100()
{
	const uintptr_t hitObjectManagerInstance = GetInstance();

	return hitObjectManagerInstance ? *reinterpret_cast<int*>(hitObjectManagerInstance + HITOBJECTMANAGER_HITWINDOW100_OFFSET) : 0;
}

int HitObjectManager::GetHitWindow300()
{
	const uintptr_t hitObjectManagerInstance = GetInstance();

	return hitObjectManagerInstance ? *reinterpret_cast<int*>(hitObjectManagerInstance + HITOBJECTMANAGER_HITWINDOW300_OFFSET) : 0;
}

float HitObjectManager::GetSpriteDisplaySize()
{
	const uintptr_t hitObjectManagerInstance = GetInstance();

	return hitObjectManagerInstance ? *reinterpret_cast<float*>(hitObjectManagerInstance + HITOBJECTMANAGER_SPRITEDISPLAYSIZE_OFFSET) : 0.f;
}

void HitObjectManager::SetSpriteDisplaySize(float value)
{
	if (const uintptr_t hitObjectManagerInstance = GetInstance())
		*reinterpret_cast<float*>(hitObjectManagerInstance + HITOBJECTMANAGER_SPRITEDISPLAYSIZE_OFFSET) = value;
}

float HitObjectManager::GetHitObjectRadius()
{
	const uintptr_t hitObjectManagerInstance = GetInstance();

	return hitObjectManagerInstance ? *reinterpret_cast<float*>(hitObjectManagerInstance + HITOBJECTMANAGER_HITOBJECTRADIUS_OFFSET) : 0.f;
}

void HitObjectManager::SetHitObjectRadius(float value)
{
	if (const uintptr_t hitObjectManagerInstance = GetInstance())
		*reinterpret_cast<float*>(hitObjectManagerInstance + HITOBJECTMANAGER_HITOBJECTRADIUS_OFFSET) = value;
}

float HitObjectManager::GetSpriteRatio()
{
	const uintptr_t hitObjectManagerInstance = GetInstance();

	return hitObjectManagerInstance ? *reinterpret_cast<float*>(hitObjectManagerInstance + HITOBJECTMANAGER_SPRITERATIO_OFFSET) : 0.f;
}

void HitObjectManager::GetSpriteRatio(float value)
{
	if (const uintptr_t hitObjectManagerInstance = GetInstance())
		*reinterpret_cast<float*>(hitObjectManagerInstance + HITOBJECTMANAGER_SPRITERATIO_OFFSET) = value;
}

uintptr_t HitObjectManager::GetSpriteManagerInstance()
{
	const uintptr_t hitObjectManagerInstance = GetInstance();

	return hitObjectManagerInstance ? *reinterpret_cast<uintptr_t*>(hitObjectManagerInstance + HITOBJECTMANAGER_SPRITEMANAGER_OFFSET) : 0u;
}

float HitObjectManager::GetGamefieldSpriteRatio()
{
	const uintptr_t spriteManagerInstance = GetSpriteManagerInstance();

	return spriteManagerInstance ? SpriteManager::GetGamefieldSpriteRatio(spriteManagerInstance) : 0.f;
}

void HitObjectManager::SetGamefieldSpriteRatio(float value)
{
	if (const uintptr_t spriteManagerInstance = GetSpriteManagerInstance())
		SpriteManager::SetGamefieldSpriteRatio(spriteManagerInstance, value);
}

float HitObjectManager::GetStackOffset()
{
	const uintptr_t hitObjectManagerInstance = GetInstance();

	return hitObjectManagerInstance ? *reinterpret_cast<float*>(hitObjectManagerInstance + HITOBJECTMANAGER_STACKOFFSET_OFFSET) : 0.f;
}

void HitObjectManager::SetStackOffset(float value)
{
	if (const uintptr_t hitObjectManagerInstance = GetInstance())
		*reinterpret_cast<float*>(hitObjectManagerInstance + HITOBJECTMANAGER_STACKOFFSET_OFFSET) = value;
}

int HitObjectManager::GetCurrentHitObjectIndex()
{
	const uintptr_t hitObjectManagerInstance = GetInstance();

	return hitObjectManagerInstance ? *reinterpret_cast<int*>(hitObjectManagerInstance + HITOBJECTMANAGER_CURRENTHITOBJECTINDEX_OFFSET) : 0.f;
}

int HitObjectManager::GetHitObjectsCount()
{
	const uintptr_t hitObjectManagerInstance = GetInstance();

	return hitObjectManagerInstance ? *reinterpret_cast<int*>(hitObjectManagerInstance + HITOBJECTMANAGER_HITOBJECTSCOUNT_OFFSET) : 0.f;
}
