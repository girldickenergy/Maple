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

	if (ConfigManager::CurrentConfig.Timewarp.Enabled)
	{
		spoofPlayDuration();

		Milk::Get().AdjustRate(Timewarp::GetRateMultiplier());
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

	if (!z3Compiled)
	{
		*reinterpret_cast<uintptr_t*>(z3Slot) = reinterpret_cast<uintptr_t>(oSubmit);
		z3Compiled = true;
	}

	[[clang::musttail]] return oSubmit(instance);
}

void Score::Initialize()
{
	if (auto submitAddress = VanillaPatternScanner::FindPattern(xorstr_("68 ?? ?? ?? ?? C3 8B F1 83 BE ?? ?? ?? ?? 00"))) // Score.Submit
	{
		auto h3Address = *reinterpret_cast<uintptr_t*>(submitAddress + 0x1); // A.B.H3
		z3Slot = VanillaPatternScanner::FindPatternInRange(xorstr_("FF 15"), h3Address, 0x25, 0x2, 1); // A.B.Z3 ptr
		if (z3Slot)
		{
			oSubmit = reinterpret_cast<fnSubmit>(*reinterpret_cast<uintptr_t*>(z3Slot));
			*reinterpret_cast<uintptr_t*>(z3Slot) = reinterpret_cast<uintptr_t>(submitHook);
		}
		else
		{
			ConfigManager::BypassFailed = true;
			ConfigManager::ForceDisableScoreSubmission = true;
			Logger::Log(LogSeverity::Error, xorstr_("Failed to find A.B::Z3 slot!"));
		}
	}
	else
	{
		ConfigManager::BypassFailed = true;
		ConfigManager::ForceDisableScoreSubmission = true;
		Logger::Log(LogSeverity::Error, xorstr_("Failed to find Score::Submit!"));
	}
}

void Score::FixSubmitHook()
{
	if (!submitHookFixed && z3Compiled)
	{
		oSubmit = reinterpret_cast<fnSubmit>(*reinterpret_cast<uintptr_t*>(z3Slot));
		*reinterpret_cast<uintptr_t*>(z3Slot) = reinterpret_cast<uintptr_t>(submitHook);
		submitHookFixed = true;
	}
}

void Score::Submit()
{
	if (!z3Compiled)
	{
		*reinterpret_cast<uintptr_t*>(z3Slot) = reinterpret_cast<uintptr_t>(oSubmit);
		z3Compiled = true;
	}

	oSubmit(scoreInstance);

	Vanilla::RemoveRelocation(std::ref(scoreInstance));
}

void Score::AbortSubmission()
{
	Vanilla::RemoveRelocation(std::ref(scoreInstance));
}
