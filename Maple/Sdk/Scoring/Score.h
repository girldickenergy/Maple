#pragma once

#include <cstdint>

class Score
{
	static inline uintptr_t scoreInstance;

	static void checkFlags();
	static void handleScoreSubmissionPrompt();

	typedef void(__fastcall* fnSubmit)(uintptr_t instance);
	static inline fnSubmit oSubmit;
	static void __fastcall submitHook(uintptr_t instance);
public:
	static void Initialize();

	static void Submit();
	static void AbortSubmission();
};
