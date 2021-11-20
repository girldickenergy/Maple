#pragma once

class ScoreSubmission
{
	typedef void(__fastcall* fnSubmit)(void* instance);

	static inline bool canRestoreGC = true;
	static inline void* scoreInstance = nullptr;
public:
	static inline fnSubmit oSubmit;
	static void __fastcall SubmitHook(void* instance);
	static void Submit();
	static void AbortSubmission();
};