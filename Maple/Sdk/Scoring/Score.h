#pragma once

#include <cstdint>

class Score
{
	static inline uintptr_t scoreInstance;

	typedef void(__fastcall* fnSubmit)(uintptr_t instance);
	static inline fnSubmit oSubmit;
	static void __fastcall submitHook(uintptr_t instance);
public:
	static void Initialize();

	static void Submit();
	static void AbortSubmission();
};
