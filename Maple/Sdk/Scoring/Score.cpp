#include "Score.h"

#include "Milk.h"
#include "Vanilla.h"
#include "xorstr.hpp"

#include "../../Configuration/ConfigManager.h"
#include "../../Features/Timewarp/Timewarp.h"
#include "../Player/Player.h"
#include "../../UI/Windows/ScoreSubmissionDialog.h"
#include "../../Logging/Logger.h"
#include "../Mods/ModManager.h"
#include "PatternScanning/VanillaPatternScanner.h"
#include "../Osu/GameBase.h"
#include "Hooking/VanillaHooking.h"
#include "../Memory.h"

void Score::spoofPlayDuration()
{
	const int playStartTime = *reinterpret_cast<int*>(scoreInstance + STARTTIME_OFFSET);
	const int playEndTime = GameBase::GetTime();
	const int playDuration = playEndTime - playStartTime;
	const int scaledDuration = static_cast<int>(playDuration * Timewarp::GetRateMultiplier());
	*reinterpret_cast<int*>(scoreInstance + STARTTIME_OFFSET) = playEndTime - scaledDuration;
}

void __fastcall Score::submitHook(uintptr_t instance)
{
	scoreInstance = instance;

	if (!playerInitChecked && !Player::HasInitialized())
	{
		Logger::Log(LogSeverity::Error, xorstr_("Critical Player field(s) was (were) not found!"));

		playerInitChecked = true;
		ConfigManager::BypassFailed = true;
		ConfigManager::ForceDisableScoreSubmission = true;
	}

	if (ConfigManager::CurrentConfig.Timewarp.Enabled)
	{
		spoofPlayDuration();

		Milk::Get().AdjustRate(Timewarp::GetRateMultiplier(), !ModManager::CheckActive(Mods::HalfTime) && !ModManager::CheckActive(Mods::DoubleTime) && !ModManager::CheckActive(Mods::Nightcore));
		Milk::Get().AdjustPollingVectorsToRate(Timewarp::GetRateMultiplier());
	}

	if (Player::GetPlayMode() == PlayModes::Osu && ModManager::CheckActive(Mods::Hidden) && ConfigManager::CurrentConfig.Visuals.Removers.HiddenRemoverEnabled)
		Milk::Get().SetSpriteCollectionCounts(3);

	if (Milk::Get().IsBroken())
	{
		ConfigManager::BypassFailed = true;
		ConfigManager::ForceDisableScoreSubmission = true;

		Logger::Log(LogSeverity::Error, xorstr_("rv.empty() || sccv.empty()!"));
	}

	if (Player::GetAnticheatFlag() != 0)
		Logger::Log(LogSeverity::Warning, xorstr_("AC flag is not zero! Flag -> %d"), Player::GetAnticheatFlag());

	Player::ResetAnticheatFlag();

	if (ConfigManager::CurrentConfig.Misc.ScoreSubmissionType == 1 || ConfigManager::ForceDisableScoreSubmission)
		return;

	if (ConfigManager::CurrentConfig.Misc.ScoreSubmissionType == 2 && !Player::GetIsRetrying())
	{
		Vanilla::AddRelocation(std::ref(scoreInstance));

		ScoreSubmissionDialog::Show();

		return;
	}

	if (ConfigManager::CurrentConfig.Misc.ScoreSubmissionType == 2 && Player::GetIsRetrying() && ConfigManager::CurrentConfig.Misc.PromptBehaviorOnRetry == 1)
		return;

	[[clang::musttail]] return oSubmit(instance);
}

void Score::Initialize()
{
	Memory::AddObject(xorstr_("A.B.H3"), xorstr_("68 ?? ?? ?? ?? C3 8B F1 83 BE ?? ?? ?? ?? 00"), 0x1, 0x1);
	Memory::AddHook(xorstr_("A.B.H3_Hook"), xorstr_("A.B.H3"), reinterpret_cast<uintptr_t>(submitHook), reinterpret_cast<uintptr_t*>(&oSubmit));

	if (!Memory::Objects[xorstr_("A.B.H3")])
	{
		Logger::Log(LogSeverity::Error, xorstr_("Failed to hook A.B.H3!"));

		ConfigManager::BypassFailed = true;
		ConfigManager::ForceDisableScoreSubmission = true;
	}

	Memory::AddObject(xorstr_("Score::Instance"), xorstr_("55 8B EC 57 56 83 EC 1C A1"), 0x9, 0x1);
}

void Score::Submit()
{
	oSubmit(scoreInstance);

	Vanilla::RemoveRelocation(std::ref(scoreInstance));
}

void Score::AbortSubmission()
{
	Vanilla::RemoveRelocation(std::ref(scoreInstance));
}

uintptr_t Score::GetInstance()
{
	const uintptr_t instanceAddress = Memory::Objects[xorstr_("Score::Instance")];

	return instanceAddress ? *reinterpret_cast<uintptr_t*>(instanceAddress) : 0u;
}

int Score::GetScore()
{
	const uintptr_t instance = GetInstance();
	if (!instance)
		return 0;

	return *reinterpret_cast<int*>(instance + TOTAL_SCORE_OFFSET);
}

int Score::GetCombo()
{
	const uintptr_t instance = GetInstance();
	if (!instance)
		return 0;

	return *reinterpret_cast<int*>(instance + CURRENT_COMBO_OFFSET);
}

float Score::GetAccuracy()
{
	const uintptr_t instance = GetInstance();
	if (!instance)
		return 0.f;

	auto count300 = *reinterpret_cast<uint16_t*>(instance + COUNT300_OFFSET);
	auto count100 = *reinterpret_cast<uint16_t*>(instance + COUNT100_OFFSET);
	auto count50 = *reinterpret_cast<uint16_t*>(instance + COUNT50_OFFSET);
	auto countMiss = *reinterpret_cast<uint16_t*>(instance + COUNTMISS_OFFSET);

	auto totalHits = count300 + count100 + count50 + countMiss;
	if (totalHits <= 0)
	{
		if (Player::GetPlayMode() == PlayModes::Taiko)
			return 0.f;

		return 100.f;
	}

	return static_cast<float>(Player::GetPlayMode() == PlayModes::Taiko ? count300 * 300 + count100 * 150 : count300 * 300 + count100 * 100 + count50 * 50) / static_cast<float>(totalHits * 300) * 100.f;
}
