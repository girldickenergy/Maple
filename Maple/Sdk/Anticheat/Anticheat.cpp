#include "Anticheat.h"

#include <Vanilla.h>

#include "../Osu/GameBase.h"

void Anticheat::Initialize()
{
	Method gameBaseFreeAC = Vanilla::Explorer["osu.GameBase"]["FreeAC"].Method;
	gameBaseFreeAC.Compile();
	freeAC = static_cast<fnFreeAC>(gameBaseFreeAC.GetNativeStart());
	
	Method playerAllowSubmissionVariableConditions = Vanilla::Explorer["osu.GameModes.Play.Player"]["get_AllowSubmissionVariableConditions"].Method;
	Method playerHandleScoreSubmission = Vanilla::Explorer["osu.GameModes.Play.Player"]["HandleScoreSubmission"].Method;
	Method playerUpdate = Vanilla::Explorer["osu.GameModes.Play.Player"]["Update"].Method;
	Method playerCheckFlashlightHax = Vanilla::Explorer["osu.GameModes.Play.Player"]["checkFlashlightHax"].Method;
	Method playerHaxCheckMouse = Vanilla::Explorer["osu.GameModes.Play.Player"]["HaxCheckMouse"].Method;
	Method playerCheckAimAssist = Vanilla::Explorer["osu.GameModes.Play.Player"]["CheckAimAssist"].Method;
	Method rulesetIncreaseScoreHit = Vanilla::Explorer["osu.GameModes.Play.Rulesets.Ruleset"]["IncreaseScoreHit"].Method;

	playerAllowSubmissionVariableConditions.Compile();
	playerHandleScoreSubmission.Compile();
	playerUpdate.Compile();
	playerCheckFlashlightHax.Compile();
	playerHaxCheckMouse.Compile();
	playerCheckAimAssist.Compile();
	rulesetIncreaseScoreHit.Compile();

	playerAllowSubmissionVariableConditionsAddress = playerAllowSubmissionVariableConditions.GetNativeStart();
	playerHandleScoreSubmissionAddress = playerHandleScoreSubmission.GetNativeStart();
	playerUpdateAddress = playerUpdate.GetNativeStart();
	playerCheckFlashlightHaxAddress = playerCheckFlashlightHax.GetNativeStart();
	playerHaxCheckMouseAddress = playerHaxCheckMouse.GetNativeStart();
	playerCheckAimAssistAddress = playerCheckAimAssist.GetNativeStart();
	rulesetIncreaseScoreHitAddress = rulesetIncreaseScoreHit.GetNativeStart();
	
	acFlagAddress = Vanilla::Explorer["osu.GameModes.Play.Player"]["flag"].Field.GetAddress();
}

void Anticheat::DisableAnticheat()
{
	#ifdef _DEBUG
		freeAC(GameBase::Instance());
	#endif
	
	Vanilla::InstallNOPPatch("PlayerAllowSubmissionVariableConditions", reinterpret_cast<uintptr_t>(playerAllowSubmissionVariableConditionsAddress), "\x83\xBE\x00\x00\x00\x00\x00\x7E\x1C", "xx????xxx", 0x80, 0, 27);
	Vanilla::InstallPatch("PlayerHandleScoreSubmission", reinterpret_cast<uintptr_t>(playerHandleScoreSubmissionAddress), "\x80\x78\x7C\x00\x0F\x84", "xxxxxx", 0x40F, 5, "\x8D");
	Vanilla::InstallPatch("PlayerUpdate", reinterpret_cast<uintptr_t>(playerUpdateAddress), "\x0F\x85\x00\x00\x00\x00\x83\xBE\x00\x00\x00\x00\xFF", "xx????xx????x", 0x1BCC, 1, "\x8D");
	Vanilla::InstallPatch("PlayerCheckFlashlightHax", reinterpret_cast<uintptr_t>(playerCheckFlashlightHaxAddress), "\x80\x3D\x00\x00\x00\x00\x00\x75\x24\x83\xBF", "xx????xxxxx", 0x375, 7, "\x7D");
	Vanilla::InstallPatch("PlayerHaxCheckMouse", reinterpret_cast<uintptr_t>(playerHaxCheckMouseAddress), "\x80\x3D\x00\x00\x00\x00\x00\x75\x14", "xx????xxx", 0x2C0, 7, "\x7D");
	Vanilla::InstallPatch("PlayerCheckAimAssist", reinterpret_cast<uintptr_t>(playerCheckAimAssistAddress), "\x80\x38\x00\x74\x0B", "xxxxx", 0x212, 3, "\x7D");
	Vanilla::InstallPatch("RulesetIncreaseScoreHit", reinterpret_cast<uintptr_t>(rulesetIncreaseScoreHitAddress), "\x80\x78\x7C\x00\x0F\x84", "xxxxxx", 0x1D36, 5, "\x8D");
}

void Anticheat::EnableAnticheat()
{
	Vanilla::RemovePatch("PlayerAllowSubmissionVariableConditions");
}

int Anticheat::GetFlag()
{
	return *static_cast<bool*>(acFlagAddress);
}
