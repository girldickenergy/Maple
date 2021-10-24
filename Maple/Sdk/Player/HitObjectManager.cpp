#include "HitObjectManager.h"

#include <Vanilla.h>

#include "Player.h"

void HitObjectManager::Initialize()
{
	RawHitObjectManager   = Vanilla::Explorer["osu.GameplayElements.HitObjectManager"];
	RawHitObject		  = Vanilla::Explorer["osu.GameplayElements.HitObjects.HitObject"];
	RawHitObjectSliderOsu = Vanilla::Explorer["osu.GameplayElements.HitObjects.Osu.SliderOsu"];

	hitObjectManagerField				 = Player::RawPlayer["hitObjectManager"].Field;
	preEmptField						 = RawHitObjectManager["PreEmpt"].Field;
	preEmptSliderCompleteField			 = RawHitObjectManager["PreEmptSliderComplete"].Field;
	activeModsField						 = RawHitObjectManager["ActiveMods"].Field;
	hitWindow300Field					 = RawHitObjectManager["HitWindow300"].Field;
	hitWindow100Field					 = RawHitObjectManager["HitWindow100"].Field;
	hitWindow50Field					 = RawHitObjectManager["HitWindow50"].Field;
	hitObjectRadiusField				 = RawHitObjectManager["HitObjectRadius"].Field;
	hitObjectTypeField					 = RawHitObject["Type"].Field;
	hitObjectStartTimeField				 = RawHitObject["StartTime"].Field;
	hitObjectEndTimeField				 = RawHitObject["EndTime"].Field;
	hitObjectPositionField				 = RawHitObject["Position"].Field;
	hitObjectEndPositionField			 = RawHitObjectSliderOsu["<EndPosition>k__BackingField"].Field;
	hitObjectSegmentCountField			 = RawHitObject["SegmentCount"].Field;
	hitObjectSpatialLengthField			 = RawHitObject["SpatialLength"].Field;
	hitObjectSliderCurvePointsField		 = RawHitObjectSliderOsu["sliderCurvePoints"].Field;
	hitObjectCumulativeLengthsField		 = RawHitObjectSliderOsu["cumulativeLength"].Field;
	hitObjectSliderCurveSmoothLinesField = RawHitObjectSliderOsu["sliderCurveSmoothLines"].Field;

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

	HitObjectType type	 = *reinterpret_cast<HitObjectType*>(hitObjectInstance + hitObjectTypeField.GetOffset());
	int startTime		 = *reinterpret_cast<int*>(hitObjectInstance + hitObjectStartTimeField.GetOffset());
	int endTime			 = *reinterpret_cast<int*>(hitObjectInstance + hitObjectEndTimeField.GetOffset());
	Vector2 position	 = *reinterpret_cast<Vector2*>(hitObjectInstance + hitObjectPositionField.GetOffset());
	int segmentCount	 = *reinterpret_cast<int*>(hitObjectInstance + hitObjectSegmentCountField.GetOffset());
	double spatialLength = *reinterpret_cast<double*>(hitObjectInstance + hitObjectSpatialLengthField.GetOffset());

	if ((type & HitObjectType::Slider) > HitObjectType::None)
	{
		Vector2 endPosition = *reinterpret_cast<Vector2*>(hitObjectInstance + hitObjectEndPositionField.GetOffset());

		std::vector<Vector2> sliderCurvePoints;
		std::vector<std::pair<Vector2, Vector2>> sliderCurveSmoothLines;
		std::vector<double> cumulativeLengths;

		uintptr_t sliderCurvePointsPointer = *reinterpret_cast<uintptr_t*>(hitObjectInstance + hitObjectSliderCurvePointsField.GetOffset());
		uintptr_t sliderCurvePointsItems   = *reinterpret_cast<uintptr_t*>(sliderCurvePointsPointer + 0x04);
		int sliderCurvePointsCount		   = *reinterpret_cast<int*>(sliderCurvePointsItems + 0x0C);

		for (uintptr_t i = 0, item = *reinterpret_cast<uintptr_t*>(sliderCurvePointsItems + 0x08 + 0x04 * i); i < sliderCurvePointsCount; i++)
		{
			Vector2 point1 = *reinterpret_cast<Vector2*>(item + 0x08);
			Vector2 point2 = *reinterpret_cast<Vector2*>(item + 0x10);

			sliderCurvePoints.emplace_back(point1, point2);
		}

		uintptr_t sliderCurveSmoothLinesPointer = *reinterpret_cast<uintptr_t*>(hitObjectInstance + hitObjectSliderCurveSmoothLinesField.GetOffset());
		uintptr_t sliderCurveSmoothLinesItems   = *reinterpret_cast<uintptr_t*>(sliderCurveSmoothLinesPointer + 0x04);
		int sliderCurveSmoothLinesCount			= *reinterpret_cast<int*>(sliderCurveSmoothLinesPointer + 0x0C);

		for (uintptr_t i = 0, item = *reinterpret_cast<uintptr_t*>(sliderCurveSmoothLinesItems + 0x08 + 0x04 * i); i < sliderCurveSmoothLinesCount; i++)
		{
			Vector2 point1 = *reinterpret_cast<Vector2*>(item + 0x08);
			Vector2 point2 = *reinterpret_cast<Vector2*>(item + 0x10);

			sliderCurveSmoothLines.emplace_back(point1, point2);
		}

		uintptr_t cumulativeLengthsPointer = *reinterpret_cast<uintptr_t*>(hitObjectInstance + hitObjectCumulativeLengthsField.GetOffset());
		uintptr_t cumulativeLengthsItems   = *reinterpret_cast<uintptr_t*>(cumulativeLengthsPointer + 0x4);
		int cumulativeLengthsCount		   = *reinterpret_cast<int*>(cumulativeLengthsPointer + 0xC);

		for (double i = 0, item = *reinterpret_cast<double*>(cumulativeLengthsItems + 0x8 + 0x8 * static_cast<int>(i)); i < cumulativeLengthsCount; i++)
			cumulativeLengths.emplace_back(item);

		return HitObject(type, startTime, endTime, position, endPosition, segmentCount, spatialLength, sliderCurvePoints, sliderCurveSmoothLines, cumulativeLengths);
	}

	return HitObject(type, startTime, endTime, position, position, segmentCount, spatialLength);
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
