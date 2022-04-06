#include "Anticheat.h"

#include <ThemidaSDK.h>
#include <Vanilla.h>

#include "../../Logging/Logger.h"
#include "../../Utilities/Security/xorstr.hpp"
#include "../Osu/GameBase.h"

void Anticheat::Initialize()
{
	VM_SHARK_BLACK_START
	STR_ENCRYPT_START
	
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
	Method playerHaxCheckPass = Vanilla::Explorer["osu.GameModes.Play.Player"]["HaxCheckPass"].Method;

	playerAllowSubmissionVariableConditions.Compile();
	playerHandleScoreSubmission.Compile();
	playerUpdate.Compile();
	playerCheckFlashlightHax.Compile();
	playerHaxCheckMouse.Compile();
	playerCheckAimAssist.Compile();
	rulesetIncreaseScoreHit.Compile();
	playerHaxCheckPass.Compile();

	playerAllowSubmissionVariableConditionsAddress = playerAllowSubmissionVariableConditions.GetNativeStart();
	playerHandleScoreSubmissionAddress = playerHandleScoreSubmission.GetNativeStart();
	playerUpdateAddress = playerUpdate.GetNativeStart();
	playerCheckFlashlightHaxAddress = playerCheckFlashlightHax.GetNativeStart();
	playerHaxCheckMouseAddress = playerHaxCheckMouse.GetNativeStart();
	playerCheckAimAssistAddress = playerCheckAimAssist.GetNativeStart();
	rulesetIncreaseScoreHitAddress = rulesetIncreaseScoreHit.GetNativeStart();
	playerHaxCheckPassAddress = playerHaxCheckPass.GetNativeStart();
	
	acFlagAddress = Vanilla::Explorer["osu.GameModes.Play.Player"]["flag"].Field.GetAddress();

	VM_SHARK_BLACK_END
	STR_ENCRYPT_END
}

void Anticheat::DisableAnticheat()
{
	VM_SHARK_BLACK_START
	STR_ENCRYPT_START
	
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
	Vanilla::InstallNOPPatch("PlayerHaxCheckPass", reinterpret_cast<uintptr_t>(playerHaxCheckPassAddress), "\xEB\x02\x33\xC0\x85\xC0\x75\x13", "xxxxxxxx", 0x400, 0x8, 7);

	STR_ENCRYPT_END
	VM_SHARK_BLACK_END
}

void Anticheat::EnableAnticheat()
{
	VM_SHARK_BLACK_START
	STR_ENCRYPT_START
	
	Vanilla::RemovePatch("PlayerAllowSubmissionVariableConditions");
	Vanilla::RemovePatch("PlayerHandleScoreSubmission");
	Vanilla::RemovePatch("PlayerUpdate");
	Vanilla::RemovePatch("PlayerCheckFlashlightHax");
	Vanilla::RemovePatch("PlayerHaxCheckMouse");
	Vanilla::RemovePatch("PlayerCheckAimAssist");
	Vanilla::RemovePatch("RulesetIncreaseScoreHit");
	Vanilla::RemovePatch("PlayerHaxCheckPass");

	STR_ENCRYPT_END
	VM_SHARK_BLACK_END
}

int Anticheat::GetFlag()
{
	return *static_cast<int*>(acFlagAddress);
}

void Anticheat::ResetFlag()
{
	*static_cast<int*>(acFlagAddress) = 0;
}
