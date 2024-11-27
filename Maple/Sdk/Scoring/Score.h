#pragma once

#include <cstdint>

class Score
{
	static inline constexpr int STARTTIME_OFFSET = 0x74;
	static inline constexpr int TOTAL_SCORE_OFFSET = 0x78;
	static inline constexpr int CURRENT_COMBO_OFFSET = 0x94;
	static inline constexpr int COUNT300_OFFSET = 0x8A;
	static inline constexpr int COUNT100_OFFSET = 0x88;
	static inline constexpr int COUNT50_OFFSET = 0x8C;
	static inline constexpr int COUNTMISS_OFFSET = 0x92;

	static void spoofPlayDuration();

	static inline uintptr_t scoreInstance;
	static inline bool playerInitChecked = false;

	typedef void(__fastcall* fnSubmit)(uintptr_t instance);
	static inline fnSubmit oSubmit;
	static void __fastcall submitHook(uintptr_t instance);
public:
	static void Initialize();

	static void Submit();
	static void AbortSubmission();

	static uintptr_t GetInstance();
	static int GetScore();
	static int GetCombo();
	static float GetAccuracy();
};
