#pragma once

typedef void(__thiscall* fnFreeAC)(void* instance);

class Anticheat
{
	static inline fnFreeAC freeAC;
	
	static inline void* playerAllowSubmissionVariableConditionsAddress = nullptr;
	static inline void* playerHandleScoreSubmissionAddress = nullptr;
	static inline void* playerUpdateAddress = nullptr;
	static inline void* playerCheckFlashlightHaxAddress = nullptr;
	static inline void* playerHaxCheckMouseAddress = nullptr;
	static inline void* playerCheckAimAssistAddress = nullptr;
	static inline void* rulesetIncreaseScoreHitAddress = nullptr;
	
	static inline void* acFlagAddress = nullptr;
public:
	static void Initialize();
	static void DisableAnticheat();
	static void EnableAnticheat();
	static int GetFlag();
};