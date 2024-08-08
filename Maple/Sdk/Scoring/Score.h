#pragma once

#include <cstdint>

class Score
{
	static inline uintptr_t scoreInstance;
	static inline uintptr_t z3Slot;
	static inline bool z3Compiled = false;
	static inline bool submitHookFixed = false;

	typedef void(__fastcall* fnSubmit)(uintptr_t instance);
	static inline fnSubmit oSubmit;
	static void __fastcall submitHook(uintptr_t instance);
public:
	static void Initialize();

	static void FixSubmitHook();
	static void Submit();
	static void AbortSubmission();
};
