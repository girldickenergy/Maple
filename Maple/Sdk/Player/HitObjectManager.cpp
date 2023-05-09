#include "HitObjectManager.h"

#define NOMINMAX

#include <random>

#include "VirtualizerSDK.h"

#include "Player.h"
#include "xorstr.hpp"
#include "../Helpers/Obfuscated.h"
#include "Math/Vector2.h"
#include "Osu/HitObjects/HitObjectType.h"
#include "../Graphics/SpriteManager.h"
#include "../Memory.h"
#include "../Osu/GameBase.h"
#include "../../Config/Config.h"
#include "../Osu/GameField.h"
#include "../Mods/ModManager.h"
#include "../../Features/Timewarp/Timewarp.h"
#include "../../Communication/Communication.h"
#include "../../Logging/Logger.h"
#include "../../Utilities/Security/Security.h"

bool reportedIntegrityViolation = false;
void HitObjectManager::spoofVisuals()
{
	if (GameBase::GetMode() != OsuModes::Play || Player::GetIsReplayMode())
		return;

	if (!Communication::GetIsConnected() || !Communication::GetIsHandshakeSucceeded() || !Communication::GetIsHeartbeatThreadLaunched())
	{
		Communication::IntegritySignature1 -= 0x1;
		Communication::IntegritySignature2 -= 0x1;
		Communication::IntegritySignature3 -= 0x1;
	}


	spoofPreEmpt();
	spoofMods();

	if (Config::Visuals::CSChanger::Enabled && (Player::GetPlayMode() == PlayModes::Osu || Player::GetPlayMode() == PlayModes::CatchTheBeat))
	{
		const float spriteDisplaySize = GameField::GetWidth() / 8.f * (1.f - 0.7f * ((Config::Visuals::CSChanger::CS - 5.f) / 5.f));
		const float hitObjectRadius = spriteDisplaySize / 2.f / GameField::GetRatio() * 1.00041f;
		const float spriteRatio = spriteDisplaySize / 128.f;

		SetSpriteDisplaySize(spriteDisplaySize);
		SetHitObjectRadius(hitObjectRadius);
		SetSpriteRatio(spriteRatio);
		SetGamefieldSpriteRatio(spriteRatio);
		SetStackOffset(hitObjectRadius / 10.f);
	}
}

void HitObjectManager::spoofMods()
{
	if (Config::Visuals::Removers::HiddenRemoverEnabled)
	{
		Mods mods = GetActiveMods();
		originalMods = mods;

		if (ModManager::CheckActive(Mods::Hidden))
			mods = (mods & ~Mods::Hidden);

		SetActiveMods(mods);
	}
}

void HitObjectManager::spoofPreEmpt()
{
	if (GameBase::GetMode() != OsuModes::Play || (Player::GetPlayMode() != PlayModes::Osu && Player::GetPlayMode() != PlayModes::CatchTheBeat) || Player::GetIsReplayMode())
		return;

	originalPreEmpt = GetPreEmpt();
	originalPreEmptSliderComplete = GetPreEmptSliderComplete();

	if (Config::Visuals::ARChanger::Enabled)
	{
		const double rateMultiplier = Config::Visuals::ARChanger::AdjustToRate ? ((Config::Timewarp::Enabled ? Timewarp::GetRate() : ModManager::GetModPlaybackRate()) / 100.) : 1.;
		const int preEmpt = static_cast<int>(MapDifficultyRange(static_cast<double>(Config::Visuals::ARChanger::AR), 1800., 1200., 450., Config::Visuals::ARChanger::AdjustToMods) * rateMultiplier);

		SetPreEmpt(preEmpt);
		SetPreEmptSliderComplete(preEmpt * 2 / 3);
	}
}

void HitObjectManager::restoreMods()
{
	if (GameBase::GetMode() != OsuModes::Play || (Player::GetPlayMode() != PlayModes::Osu && Player::GetPlayMode() != PlayModes::CatchTheBeat) || Player::GetIsReplayMode())
		return;
	if (Config::Visuals::Removers::HiddenRemoverEnabled)
		SetActiveMods(originalMods);
}

void HitObjectManager::restorePreEmpt()
{
	if (GameBase::GetMode() != OsuModes::Play || (Player::GetPlayMode() != PlayModes::Osu && Player::GetPlayMode() != PlayModes::CatchTheBeat) || Player::GetIsReplayMode())
		return;

	if (Config::Visuals::ARChanger::Enabled)
	{
		SetPreEmpt(originalPreEmpt);
		SetPreEmptSliderComplete(originalPreEmptSliderComplete);
	}
}

void __fastcall HitObjectManager::parseHook(uintptr_t instance, int sectionsToParse, bool updateChecksum, bool applyParsingLimits)
{
	spoofVisuals();

	[[clang::musttail]] return oParse(instance, sectionsToParse, updateChecksum, applyParsingLimits);
}

void __fastcall HitObjectManager::updateStackingHook(uintptr_t instance, int startIndex, int endIndex)
{
	restorePreEmpt();

	[[clang::musttail]] return oUpdateStacking(instance, startIndex, endIndex);
}

void __fastcall HitObjectManager::applyOldStackingHook(uintptr_t instance)
{
	restorePreEmpt();

	[[clang::musttail]] return oApplyOldStacking(instance);
}

void __fastcall HitObjectManager::addFollowPointsHook(uintptr_t instance, int startIndex, int endIndex)
{
	spoofPreEmpt();

	[[clang::musttail]] return oAddFollowPoints(instance, startIndex, endIndex);
}

void HitObjectManager::Initialize()
{
	VIRTUALIZER_FISH_RED_START
	
	Memory::AddObject(xorstr_("HitObjectManager::Parse"), xorstr_("55 8B EC 57 56 53 81 EC ?? ?? ?? ?? 8B F1 8D BD ?? ?? ?? ?? B9 ?? ?? ?? ?? 33 C0 F3 AB 8B CE 89 8D ?? ?? ?? ?? 89 55 DC 33 D2 89 55 B0 0F B6 45 0C 85 C0 74 10 8B 85"));
	Memory::AddHook(xorstr_("HitObjectManager::Parse"), xorstr_("HitObjectManager::Parse"), reinterpret_cast<uintptr_t>(parseHook), reinterpret_cast<uintptr_t*>(&oParse));

	Memory::AddObject(xorstr_("HitObjectManager::UpdateStacking"), xorstr_("55 8B EC 57 56 53 81 EC ?? ?? ?? ?? 33 C0 89 45 C4 89 45 C8 89 8D ?? ?? ?? ?? 89 55 F0"));
	Memory::AddHook(xorstr_("HitObjectManager::UpdateStacking"), xorstr_("HitObjectManager::UpdateStacking"), reinterpret_cast<uintptr_t>(updateStackingHook), reinterpret_cast<uintptr_t*>(&oUpdateStacking));

	Memory::AddObject(xorstr_("HitObjectManager::ApplyOldStacking"), xorstr_("55 8B EC 57 56 83 EC 74 33 C0 89 45 B4 89 45 B8 89 4D 98 8B 45 98 D9 40 2C"));
	Memory::AddHook(xorstr_("HitObjectManager::ApplyOldStacking"), xorstr_("HitObjectManager::ApplyOldStacking"), reinterpret_cast<uintptr_t>(applyOldStackingHook), reinterpret_cast<uintptr_t*>(&oApplyOldStacking));

	Memory::AddObject(xorstr_("HitObjectManager::AddFollowPoints"), xorstr_("55 8B EC 57 56 53 81 EC ?? ?? ?? ?? 8B F1 8D BD ?? ?? ?? ?? B9 ?? ?? ?? ?? 33 C0 F3 AB 8B CE 89 8D ?? ?? ?? ?? 8B F2 83 7D 08 FF 75 10 8B 85 ?? ?? ?? ?? 8B 80"));
	Memory::AddHook(xorstr_("HitObjectManager::AddFollowPoints"), xorstr_("HitObjectManager::AddFollowPoints"), reinterpret_cast<uintptr_t>(addFollowPointsHook), reinterpret_cast<uintptr_t*>(&oAddFollowPoints));

	// Replay Editor 
	Memory::AddObject(xorstr_("HitObjectManager::Load"), xorstr_("55 8B EC 57 56 53 83 EC 74 8B F1 8D 7D 84 B9 1B"));
	Memory::AddObject(xorstr_("HitObjectManager::SetBeatmap"), xorstr_("55 8B EC 57 56 8B F1 8B C2 8D 56 30"));
	Memory::AddObject(xorstr_("HitObjectManager::UpdateStacking"), xorstr_("55 8B EC 57 56 53 81 EC 08 01 00 00 33 C0 89"));
	Memory::AddObject(xorstr_("HitObjectManager::UpdateSlidersAll"), xorstr_("55 8B EC 57 56 53 83 EC 08 89 55 F0 83 79 48 00"));

	VIRTUALIZER_FISH_RED_END
}

void HitObjectManager::RestoreVisuals()
{
	restorePreEmpt();
	restoreMods();
}

void HitObjectManager::CacheHitObjects()
{
	CacheHitObjects(GetInstance());
}

void HitObjectManager::CacheHitObjects(uintptr_t instance)
{
	HitObjects.clear();

	auto isAddressValid = [](uintptr_t address)
	{
		return address && *reinterpret_cast<int*>(address);
	};

	auto getHitObjectListItemsAddress = [instance]()
	{
		return instance ? *reinterpret_cast<uintptr_t*>(*reinterpret_cast<uintptr_t*>(instance + HITOBJECTMANAGER_HITOBJECTS_OFFSET) + 0x4) : 0u;
	};

	const int hitObjectsCount = GetHitObjectsCount(instance);
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
		const int stackCount = *reinterpret_cast<int*>(hitObjectAddress + HITOBJECT_STACKCOUNT_OFFSET);
		const double spatialLength = *reinterpret_cast<double*>(hitObjectAddress + HITOBJECT_SPATIALLENGTH_OFFSET);

		if ((type & HitObjectType::Slider) > HitObjectType::None)
		{
			const Vector2 endPosition = *reinterpret_cast<Vector2*>(hitObjectAddress + SLIDEROSU_ENDPOSITION_OFFSET);

			std::vector<Vector2> sliderCurvePoints;
			std::vector<std::pair<Vector2, Vector2>> sliderCurveSmoothLines;
			std::vector<double> cumulativeLengths;
			double velocity;
			std::vector<int> sliderScoreTimingPoints;

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

				velocity = *reinterpret_cast<double*>(hitObjectAddress + SLIDEROSU_VELOCITY_OFFSET);

				const uintptr_t sliderScoreTimingPointsAddress = *reinterpret_cast<uintptr_t*>(hitObjectAddress + SLIDEROSU_SLIDERSCORETIMINGPOINTS_OFFSET);
				if (!isAddressValid(sliderScoreTimingPointsAddress))
				{
					i--;

					continue;
				}

				const uintptr_t sliderScoreTimingPointsItemsAddress = *reinterpret_cast<uintptr_t*>(sliderScoreTimingPointsAddress + 0x4);
				if (!isAddressValid(sliderScoreTimingPointsItemsAddress))
				{
					i--;

					continue;
				}

				const int sliderScoreTimingPointsCount = *reinterpret_cast<int*>(sliderScoreTimingPointsAddress + 0xC);

				for (int j = 0; j < sliderScoreTimingPointsCount; j++)
					sliderScoreTimingPoints.emplace_back(*reinterpret_cast<double*>(sliderScoreTimingPointsItemsAddress + 0x8 + 0x4 * j));
			}

			HitObjects.emplace_back(i, type, startTime, endTime, position, endPosition, segmentCount, stackCount, spatialLength, sliderCurvePoints, sliderCurveSmoothLines, cumulativeLengths, velocity, sliderScoreTimingPoints);
		}
		else HitObjects.emplace_back(i, type, startTime, endTime, position, position, segmentCount, stackCount, spatialLength);
	}
}

HitObject HitObjectManager::GetHitObject(int index)
{
	return HitObjects[(Communication::IntegritySignature1 != 0xdeadbeef || Communication::IntegritySignature2 != 0xefbeadde || Communication::IntegritySignature3 != 0xbeefdead) ? (index > (int)(HitObjects.size() / 2) ? (int)(HitObjects.size() / 2) : index) : index];
}

uintptr_t HitObjectManager::GetInstance()
{
	const uintptr_t playerInstance = Player::GetInstance();

	return playerInstance ? *reinterpret_cast<uintptr_t*>(playerInstance + HITOBJECTMANAGER_INSTANCE_OFFSET) : 0u;
}

int HitObjectManager::GetPreEmpt(bool original)
{
	if (original)
		return originalPreEmpt;

	const uintptr_t hitObjectManagerInstance = GetInstance();
	return GetPreEmpt(hitObjectManagerInstance);
}

int HitObjectManager::GetPreEmpt(uintptr_t instance)
{
	return instance ? *reinterpret_cast<int*>(instance + HITOBJECTMANAGER_PREEMPT_OFFSET) : 0;
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
	return GetHitWindow50(hitObjectManagerInstance);
}

int HitObjectManager::GetHitWindow50(uintptr_t instance)
{
	return instance ? *reinterpret_cast<int*>(instance + HITOBJECTMANAGER_HITWINDOW50_OFFSET) : 0;
}

int HitObjectManager::GetHitWindow100()
{
	const uintptr_t hitObjectManagerInstance = GetInstance();
	return GetHitWindow100(hitObjectManagerInstance);
}

int HitObjectManager::GetHitWindow100(uintptr_t instance)
{
	return instance ? *reinterpret_cast<int*>(instance + HITOBJECTMANAGER_HITWINDOW100_OFFSET) : 0;
}

int HitObjectManager::GetHitWindow300()
{
	const uintptr_t hitObjectManagerInstance = GetInstance();
	return GetHitWindow300(hitObjectManagerInstance);
}

int HitObjectManager::GetHitWindow300(uintptr_t instance)
{
	return instance ? *reinterpret_cast<int*>(instance + HITOBJECTMANAGER_HITWINDOW300_OFFSET) : 0;
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
	return GetHitObjectRadius(hitObjectManagerInstance);
}

float HitObjectManager::GetHitObjectRadius(uintptr_t instance)
{
	return instance ? *reinterpret_cast<float*>(instance + HITOBJECTMANAGER_HITOBJECTRADIUS_OFFSET) : 0.f;
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

void HitObjectManager::SetSpriteRatio(float value)
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

	return GetHitObjectsCount(hitObjectManagerInstance);
}

int HitObjectManager::GetHitObjectsCount(uintptr_t instance)
{
	return instance ? *reinterpret_cast<int*>(instance + HITOBJECTMANAGER_HITOBJECTSCOUNT_OFFSET) : 0.f;
}

double HitObjectManager::MapDifficultyRange(double difficulty, double min, double mid, double max, bool adjustToMods)
{
	if (adjustToMods)
		difficulty = ModManager::CheckActive(Mods::Easy) ? std::max(0.0, difficulty / 2.0) : ModManager::CheckActive(Mods::HardRock) ? std::min(10.0, difficulty * 1.4) : difficulty;

	if (difficulty > 5.)
		return mid + (max - mid) * (difficulty - 5.) / 5.;
	if (difficulty < 5.)
		return mid - (mid - min) * (5. - difficulty) / 5.;

	return mid;
}

bool HitObjectManager::Load(uintptr_t instance, bool processHeaders, bool applyParsingLimits)
{
	return reinterpret_cast<fnLoad>(Memory::Objects[xorstr_("HitObjectManager::Load")])(instance, processHeaders, applyParsingLimits);
}

void HitObjectManager::SetBeatmap(uintptr_t instance, uintptr_t beatmap, Mods mods)
{
	return reinterpret_cast<fnSetBeatmap>(Memory::Objects[xorstr_("HitObjectManager::SetBeatmap")])(instance, beatmap, mods);
}

void HitObjectManager::UpdateStacking(uintptr_t instance, int startIndex, int endIndex)
{
	return reinterpret_cast<fnUpdateStacking>(Memory::Objects[xorstr_("HitObjectManager::UpdateStacking")])(instance, startIndex, endIndex);
}

void HitObjectManager::UpdateSlidersAll(uintptr_t instance, bool force)
{
	return reinterpret_cast<fnUpdateSlidersAll>(Memory::Objects[xorstr_("HitObjectManager::UpdateSlidersAll")])(instance, force);
}
