#pragma once

class ScoreSubmission
{
	typedef void(__fastcall* fnSubmit)(void* instance);

	static inline void* scoreInstance;
public:
	static inline fnSubmit oSubmit;
	static void __fastcall SubmitHook(void* instance);
	static void Submit();
};