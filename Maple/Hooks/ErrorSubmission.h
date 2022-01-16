#pragma once

class ErrorSubmission
{
	typedef void(__fastcall* fnSubmitError)(void* err);
public:
	static inline fnSubmitError oSubmitError;
	static void __fastcall SubmitErrorHook(void* err);
};